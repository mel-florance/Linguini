#include "Router.h"
#include "WebServer.h"
#include "../core/Json.h"

using json = nlohmann::json;

void Router::parse(const std::string& directory)
{
	this->directory = directory;

	for (const auto& entry : fs::directory_iterator(directory))
	{
		if (fs::path(entry.path()).extension() == std::string_view(".json"))
		{
			std::fstream file(entry.path(), std::fstream::in);
			std::string str {
				(std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>()
			};

			try {
				auto values = json::parse(str.c_str());

				for (auto& value : values)
				{
					Route route;
					route.name = value["name"].get<std::string>();
					route.url = value["url"].get<std::string>();
					route.method = value["method"].get<std::string>();
					route.callback = value["callback"].get<std::string>();

					if (value["middlewares"].is_array()) {
						route.middlewares = value["middlewares"].get<std::vector<std::string>>();
					}

					routes.push_back(route);
				}
			}
			catch (std::exception& e) {
				ORM::logger.error(
					"ROUTER",
					"Error while parsing routes file: %s",
					entry.path().c_str()
				);
				std::cout << e.what() << std::endl;
			}
		}
	}

}

void Router::printRoutes()
{
	auto table = ORM::createDebugTable();

	table[0][0] = "Method";
	table[0][1] = "Name";
	table[0][2] = "URL";
	table[0][3] = "Middlewares";
	table[0][4] = "Controller";
	table[0][5] = "Action";

	for (int i = 0; i < routes.size(); ++i)
	{
		table[i + 1][0] = routes[i].method;
		table[i + 1][1] = routes[i].name;
		table[i + 1][2] = routes[i].url;
		table[i + 1][3] = Utils::join(routes[i].middlewares, ", ");

		auto parts = Utils::split(routes[i].callback, ".");
		table[i + 1][4] = parts[0];
		table[i + 1][5] = parts[1];
	}

	std::cout << table;
}

Response Router::dispatch(
	Request& request,
	WebServer* server
)
{
	routes.erase(
	std::remove_if(routes.begin(), routes.end(), [](const Route& route) {
		return route.callback == "App.static_directory"
			|| route.callback == "App.static_file";
	}),
	routes.end());

	for (auto& mount_point : server->mount_points)
	{
		if (request.headers.url == mount_point.first) {
			Route static_directory;
			static_directory.name = mount_point.second;
			static_directory.alias = mount_point.first;
			static_directory.url = mount_point.first;
			static_directory.method = "GET";
			static_directory.callback = "App.static_directory";
			routes.push_back(static_directory);
		}
		else if (request.headers.url.find(mount_point.first) != std::string::npos)
		{
			Route static_file;
			auto filepath = request.headers.url.substr(mount_point.first.size() + 1, request.headers.url.size());
			static_file.name = mount_point.second + '/' + filepath;
			static_file.alias = mount_point.first + '/' + filepath;
			static_file.url = "^" + mount_point.first + std::string("/([a-zA-Z0-9_\\-\\./\\ ]+)$");
			static_file.method = "GET";
			static_file.callback = "App.static_file";
			routes.push_back(static_file);
		}
	}
	
	std::vector<std::string> urls;

	for (auto& route : routes)
	{
		std::smatch matches;
		std::string str = route.url;
		auto pattern = std::string("([a-zA-Z0-9_]+)");

		while (std::regex_search(str, matches, std::regex(":" + pattern))) {
			for (auto it = matches.begin(); it != matches.end(); ++it) {
				if (it->str().at(0) != ':') {
					route.params[it->str()] = "";
					str = matches.suffix();
				}
			}
		}
	
		urls.push_back("^" + std::regex_replace(route.url, std::regex(":" + pattern), pattern) + "$");
	}

	for (int i = 0; i < urls.size(); ++i)
	{
		std::smatch matches;

		if (std::regex_search(request.headers.url, matches, std::regex(urls[i])))
		{
			if (matches.size() > 0)
			{
				if (request.headers.url == matches[0].str()
				&& routes[i].method == request.headers.method)
				{
					routes[i].bindParams(matches);

					auto target = Utils::split(routes[i].callback, ".");
					auto c = server->controllers.find(target[0]);

					if (c == server->controllers.end())
					{
						ORM::logger.warn(
							"ROUTER",
							"Controller %s not found.",
							target[0].c_str()
						);

						return Response("Unauthorized", StatusCode::UNAUTHORIZED);
					}

					auto method = c->second->methods.find(target[1]);

					if (method == c->second->methods.end()) {
						ORM::logger.warn(
							"ROUTER",
							"Method %s on controller %s not found.",
							target[1].c_str(),
							target[0].c_str()
						);

						return Response("Unauthorized", StatusCode::UNAUTHORIZED);
					}

					request.params = routes[i].params;
					request.route = routes[i];

					bool authorized = true;
					Response response("Unauthorized", StatusCode::UNAUTHORIZED);

					for (auto& name : routes[i].middlewares) {
						auto middleware = server->middlewares.find(name);

						if (middleware != server->middlewares.end()) {
							authorized = middleware->second->use(request, response);
						}
					}

					if (!authorized)
						return response;

					return c->second.get()->methods.at(method->first)(request);
				}
			}
		}
	}

	std::fstream file("./views/Error.html");
	std::string str((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

	std::regex error("\\$error"); 
	std::regex name("\\$name"); 
	std::regex version("\\$version");

	str = std::regex_replace(str, name, statusCodeToStr(StatusCode::NOT_FOUND));
	str = std::regex_replace(str, error, std::to_string(int(StatusCode::NOT_FOUND)));
	str = std::regex_replace(str, version, "OHMyServer 1.0");

	return Response(str, StatusCode::NOT_FOUND, "text/html");
}

