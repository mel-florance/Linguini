#include "Headers.h"
#include "WebServer.h"

void Headers::parse(WebServer* server, TCPListener::TCPSocket* client)
{
	// Parse websocket connection headers
	auto upgrade = entries.find("Upgrade");
	auto connection = entries.find("Connection");
	auto security_key = entries.find("Sec-WebSocket-Key");
	auto version = entries.find("Sec-WebSocket-Version");
	auto sub_protocol = entries.find("Sec-WebSocket-Protocol");

	if (upgrade != entries.end() && connection != entries.end()
	&& security_key != entries.end() && version != entries.end())
	{
		if (upgrade->second == "websocket"
			&& connection->second == "Upgrade"
			&& version->second == SUPPORTED_WEBSOCKET_VERSION)
		{
			use_websocket = true;
			std::string key = (security_key->second + std::string(WEBSOCKET_MAGIC_STRING));

			char out[SHA_DIGEST_LENGTH];
			char buffer[SHA_DIGEST_LENGTH * 2];
			SHA1((unsigned char*)key.c_str(), key.size(), (unsigned char*)out);

			socket_hash = Utils::base64_encode(out);

			if (sub_protocol->second == "json")
				client->subprotocol = TCPListener::SubProtocol::JSON;

			client->websocket = true;
			server->clients[client->socket] = client;
		}
	}
}
