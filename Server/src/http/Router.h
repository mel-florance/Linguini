#pragma once

#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <regex>
#include <optional>

#include "Status.h"
#include "../core/Utils.h"

#include "TCPListener.h"
#include "Request.h"
#include "Response.h"
#include "Route.h"

namespace fs = std::filesystem;

class WebServer;

class Router {
public:
	Router() {
	}

	typedef std::function<Response(const Request&)> RouteCallback;

	void printRoutes();
	void parse(const std::string& directory);
	Response dispatch(Request& request, WebServer* server);

	std::vector<Route> routes;
	std::string directory;
};

/* TODO 	payload << "WWW-Authenticate: Basic realm=\"User Visible Realm\"\r\n";*/
