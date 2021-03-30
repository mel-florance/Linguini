#pragma once

#include "Controller.h"
#include "../orm/ORM.h"
#include "../http/Webserver.h"
#include "../core/Logger.h"
#include "Model.h"

class StatusController : public Controller {
public:
	StatusController(WebServer* server) : Controller(server)
	{
		methods["dashboard"] = [=](const Request& request) -> Response
		{
			json data;
			data["ip"] = request.client->ip;

			auto mailer = server->getService<Mailer>("Mailer");
			data["domain"] = ORM::env.get("DOMAIN");

			Mailer::Message message = {
				"truc@virax.dev",
				"mel.florance@gmail.com",
				"I think it's working.",
				"yeaaaa !",
				"virax.csgo@gmail.com",
				"test@gmail.com"
			};

			mailer->Send("mails/action", message, data);
			
			return Response::render("status/dashboard", data);
		};

		methods["config"] = [=](const Request& request) -> Response
		{
			json data;
			data["config"] = ORM::env.toJson();
			return Response::render("status/config", data);
		};

		methods["models"] = [=](const Request& request) -> Response
		{
			json models = {};

			for (auto& model : ORM::models) {
				json object;
				object["name"] = model.first;

				json fields = {};

				for (auto& column : model.second->schema) {
					json field;
					field["name"] = column.name;
					field["type"] = QueryBuilder::fieldTypeToStr(column.type);
					field["size"] = column.size;
					field["not_null"] = column.not_null;
					field["primary"] = column.primary;
					field["increment"] = column.increment;

					fields.push_back(field);
				}

				object["fields"] = fields;
				models.push_back(object);
			}

			json data;
			data["models"] = models;

			return Response::render("status/models", data);
		};

		methods["controllers"] = [=](const Request& request) -> Response
		{
			json controllers = {};

			for (auto& ctrl : WebServer::controllers) {
				json object;
				object["name"] = ctrl.first;

				json methods = {};

				for (auto& method : ctrl.second->methods)
					methods.push_back(method.first);

				object["methods"] = methods;
				controllers.push_back(object);
			}

			json data;
			data["controllers"] = controllers;
			return Response::render("status/controllers", data);
		};

		methods["middlewares"] = [=](const Request& request) -> Response
		{
			json middlewares = {};

			for (auto& middleware : WebServer::middlewares) {
				json object;
				object["name"] = middleware.first;
				middlewares.push_back(object);
			}

			json data;
			data["middlewares"] = middlewares;
			return Response::render("status/middlewares", data);
		};

		methods["routes"] = [=](const Request& request) -> Response
		{
			json data;
			data["routes"] = {};

			std::string name;
			std::string url;
			std::string method;
			std::string callback;
			std::unordered_map<std::string, std::string> params;
			std::vector<std::string> middlewares;

			for (auto& route : server->router.routes) {
				json object;
				object["url"] = route.url;
				object["name"] = route.name;
				object["method"] = route.method;
				object["middlewares"] = route.middlewares;

				auto parts = Utils::split(route.callback, ".");
				object["controller"] = parts[0];
				object["action"] = parts[1];

				data["routes"].push_back(object);
			}

			return Response::render("status/routes", data);
		};

		methods["static_assets"] = [=](const Request& request) -> Response
		{
			json static_assets = {};

			for (auto& asset : WebServer::mount_points) {
				json object;
				object["alias"] = asset.first;
				object["directory"] = asset.second;
				static_assets.push_back(object);
			}

			json data;
			data["static_assets"] = static_assets;
			return Response::render("status/static_assets", data);
		};

		methods["logs"] = [=](const Request& request) -> Response
		{
			json logs = {};

			for (auto& log : Logger::logs) {
				json object;
				object["entries"] = {};

				if (log.first == "SERVER") {
					for (auto& entry : log.second) {
			
						std::string regex = R"~(\[([0-9\-\:\s]+)\]\[([A-Z]+)\]\s([A-Z\/0-9\.]+)\s([A-Z]+)\s([0-9]+)\s-\s([0-9A-Z.\s]+)\s-\s([0-9\.]+)\s-\s([A-Za-z0-9\?\-\_\/]+)\s-\s(.*))~";

						try {
							std::smatch  matches;
							if (std::regex_search(entry, matches, std::regex(regex)))
							{
								if (matches.size() > 0) {
									json e;
									e["time"] = matches[1].str();
									e["protocol"] = matches[3].str();
									e["method"] = matches[4].str();
									e["status"] = matches[5].str();
									e["size"] = matches[6].str();
									e["ip"] = matches[7].str();
									e["url"] = matches[8].str();
									e["agent"] = matches[9].str();

									object["entries"].push_back(e);
								}
							}
						}
						catch (std::exception& e) {
							std::cout << e.what() << std::endl;
						}
					}
				}

				object["name"] = log.first;
				logs.push_back(object);
			}

			json data;
			data["logs"] = logs;
			data["ip"] = request.client->ip;

			return Response::render("status/logs", data);
		};
	}
};
