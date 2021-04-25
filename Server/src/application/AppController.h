#pragma once

#include "Controller.h"
#include "../orm/ORM.h"
#include "../core/Json.h"

#include <chrono>
#include <iomanip>

using namespace std::chrono;
using namespace std::chrono_literals;
using json = nlohmann::json;

template<typename T>
std::time_t to_time_t(T time)
{
	auto time_point = time_point_cast<system_clock::duration>(time - T::clock::now() + system_clock::now());

	return system_clock::to_time_t(time_point);
}

class AppController : public Controller {
public:
	AppController(WebServer* server) : Controller(server)
	{
		methods["static_directory"] = [](const Request& request) -> Response
		{
			if (!fs::exists(request.route.name))
				return Response("Directory not found.", StatusCode::NOT_FOUND);

			json data;

			data["current_dir"] = request.route.alias;
			data["entries"] = std::vector<json>();

			for (const auto& entry : fs::directory_iterator(request.route.name))
			{
				if (entry.path().string().size() > 0)
				{
					try {
						auto str = fs::u8path(entry.path().string()).string();
						auto stripped = str.substr(request.route.name.size(), str.size());
						auto filename = stripped.substr(1, stripped.size());
						auto filesize = fs::file_size(str);
						std::string size;

						if (entry.is_regular_file())
							size = Utils::bytesToSize(filesize);
						else if (entry.is_directory()) {
							unsigned long long accumulator = 0;
							Utils::getFoldersize(str, accumulator);
							size = Utils::bytesToSize(accumulator);
						}

						auto ftime = fs::last_write_time(str);
						std::time_t tt = to_time_t(ftime);
						std::tm* gmt = std::gmtime(&tt);
						std::stringstream buffer;
						buffer << std::put_time(gmt, "%A, %d %B %Y %H:%M");

						json item;
						item["url"] = Utils::join({ request.route.alias, filename }, "/");
						item["name"] = filename;
						item["size"] = size;
						item["modified"] = buffer.str();
						item["is_folder"] = entry.is_directory();

						data["entries"].push_back(item);
					}
					catch (std::exception& e) {
						std::cout << e.what() << std::endl;
					}
				}
			}

			std::sort(data["entries"].begin(), data["entries"].end(),
			[](const json& lhs, const json& rhs) {
				return int(lhs["is_folder"]) > int(rhs["is_folder"]);
			});

			return Response::render("static/directories", data);
		};

		methods["static_file"] = [=](const Request& request) -> Response
		{
			if (!fs::exists(request.route.name))
				return Response("File not found.", StatusCode::NOT_FOUND);

			if (fs::is_directory(request.route.name))
				return methods["static_directory"](request);

			auto extension = fs::path(request.route.name).extension();
			auto mime_type = Utils::getMimeType(extension.string());
			auto file_size = fs::file_size(request.route.name);

			std::ifstream file(request.route.name, std::ios::out | std::ios::binary);

			char* image = new char[file_size];
			file.read(image, file_size);

			std::ostringstream payload;
			payload << "HTTP/1.1 200 OK\r\n";
			payload << "Cache-Control: no-cache, private\r\n";
			payload << "Content-Length: " + std::to_string(file_size) + "\r\n";
			payload << "Content-Type: " + Utils::trim(mime_type) + "\r\n";
			payload << "Server: OHMyServer\r\n";
			payload << "\r\n";

			SSL_write(request.client->ssl, payload.str().c_str(), payload.str().size());
			SSL_write(request.client->ssl, image, file_size);

	/*		send(request.client.socket, payload.str().c_str(), payload.str().size(), 0);
			send(request.client.socket, image, size, 0);*/
			file.close();

			Response response;
			response.status = StatusCode::OK;
			response.binary = true;
			response.data = image;

			return response;
		};
	}
};
