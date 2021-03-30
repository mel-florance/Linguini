#pragma once

#include "Controller.h"
#include "../orm/ORM.h"
#include "../core/Json.h"

using json = nlohmann::json;

class AppController : public Controller {
public:
	AppController(WebServer* server) : Controller(server)
	{
		methods["static_directory"] = [](const Request& request) -> Response
		{
			if (!fs::exists(request.route.name))
				return Response("Directory not found.", StatusCode::NOT_FOUND);

			std::string out = "<ul>";

			for (const auto& entry : fs::directory_iterator(request.route.name))
			{
				auto str = fs::u8path(entry.path().string()).string();
				auto stripped = str.substr(request.route.name.size(), str.size());
				auto filename = stripped.substr(1, stripped.size());
				out += "<li><a href=\"" + Utils::join({ request.route.alias, filename }, "/") + "\">\"" + filename + "\"</a></li>";
			}

			out += "</ul>";

			return Response(out, StatusCode::OK, "text/html");
		};

		methods["static_file"] = [=](const Request& request) -> Response
		{
			if (!fs::exists(request.route.name))
				return Response("File not found.", StatusCode::NOT_FOUND);

			if (fs::is_directory(request.route.name))
				return methods["static_directory"](request);

			auto extension = fs::path(request.route.name).extension();
			auto mime_type = Utils::getMimeType(extension.string());

			std::ifstream file(request.route.name, std::ios::in | std::ios::binary);

			if (!file.is_open())
				return Response("Cannot open file", StatusCode::INTERNAL_SERVER_ERROR);

			file.seekg(0, std::ios::end);
			std::streampos size = file.tellg();
			char* image = new char[size];
			file.seekg(0, std::ios::beg);
			file.read(image, size);

			std::ostringstream payload;
			payload << "HTTP/1.1 200 OK\r\n";
			payload << "Cache-Control: no-cache, private\r\n";
			payload << "Content-Length: " + std::to_string(int(size)) + "\r\n";
			payload << "Content-Type: " + mime_type + "\r\n";
			payload << "Server: OHMyServer\r\n";
			payload << "\r\n";

			SSL_write(request.client->ssl, payload.str().c_str(), payload.str().size());
			SSL_write(request.client->ssl, image, size);

	/*		send(request.client.socket, payload.str().c_str(), payload.str().size(), 0);
			send(request.client.socket, image, size, 0);*/
			file.close();

			Response response("Ok");
			response.binary = true;
			response.data = image;

			return response;
		};
	}
};
