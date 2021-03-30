#pragma once

#include <map>
#include <functional>

#include "../http/Router.h"

#include "../core/Inja.h"

using namespace inja;
using json = nlohmann::json;

class Controller
{
public:
	Controller(WebServer* server) {}
	virtual ~Controller() {}

	std::map<std::string, Router::RouteCallback> methods;
};

