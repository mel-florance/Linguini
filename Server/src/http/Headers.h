#pragma once

#include <string>
#include <unordered_map>
#include "../core/Utils.h"
#include <openssl/sha.h>
#include "TCPListener.h"

#define SUPPORTED_WEBSOCKET_VERSION "13"
#define WEBSOCKET_MAGIC_STRING "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

class WebServer;

struct Headers
{
	std::string url;
	std::string host;
	std::string scheme;
	std::string method;
	std::string protocol;
	std::unordered_map<std::string, std::string> entries;
	bool upgrade_websocket = false;
	bool connection_upgrade = false;
	bool use_websocket = false;
	std::string socket_key;
	std::string socket_version;
	std::string socket_hash;

	void parse(WebServer* server, TCPListener::TCPSocket* client);
};
