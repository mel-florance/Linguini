#include "WebServer.h"
#include "../application/AppController.h"

namespace fs = std::filesystem;

std::unordered_map<std::string, std::string> WebServer::mount_points = {};
std::unordered_map<std::string, std::unique_ptr<Controller>> WebServer::controllers = {};
std::unordered_map<std::string, std::unique_ptr<Middleware>> WebServer::middlewares = {};
std::unordered_map<std::string, std::shared_ptr<Service>> WebServer::services = {};

WebServer::WebServer(int port, const char* ip) :
	TCPListener(ip, port),
	logger(Logger()),
	router(Router()),
	orm(ORM()),
	uaparser(UAParser("./project/data/user-agents.json"))
{
	store = Store();

	orm.loadModels("./project/models/");
	router.parse("./project/routes");
	store.parse("./project/stores");

	for (auto route : store.http_routes)
		router.addRoute(route);

	registerController<AppController>("App");
}

void WebServer::onMessageReceived(TCPSocket* client, const char* message, int length)
{
	Request request(message);
	request.server = this;
	request.client = client;
	request.parse();

	Response response;

	if (request.headers.use_websocket)
	{
		response.status = StatusCode::SWITCHING_PROTOCOL;
		response.setHeader("Sec-WebSocket-Accept", request.headers.socket_hash);
		response.setHeader("Upgrade", "websocket");
		response.setHeader("Connection", "Upgrade");
		auto payload = response.toString();
		sendToClient(client, payload.c_str(), payload.size());
	}
	else {
		response = router.dispatch(request, this);

		if (!response.binary) {
			auto payload = response.toString();
			sendToClient(client, payload.c_str(), payload.size());
		}
	}

	logger.log(
		"HTTP",
		"%s %s %d - %s - %s - %s - %s",
		request.headers.protocol.c_str(),
		request.headers.method.c_str(),
		int(response.status),
		Utils::bytesToSize(response.body.size()).c_str(),
		request.client->ip.c_str(),
		request.headers.url.c_str(),
		uaparser.parse(request.headers.entries["User-Agent"]).c_str()
	);
}

void WebServer::onClientConnected(TCPSocket* client)
{
}

void WebServer::onClientDisconnected(TCPSocket* client)
{
}
