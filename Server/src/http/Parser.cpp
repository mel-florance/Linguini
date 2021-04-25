#include "Parser.h"
#include "../core/Utils.h"

std::shared_ptr<Request> Parser::parse(const char* str)
{
	auto parts = Utils::split(str, "\r\n\r\n");

	if (parts.size() <= 0) {
		std::cout << "Cannot parse http payload." << std::endl;
		return nullptr;
	}

	auto lines = Utils::split(parts[0], "\r\n");

	if (lines.size() <= 0) {
		std::cout << "Cannot parse http headers." << std::endl;
		return nullptr;
	}

	auto request_parts = Utils::split(lines.at(0), " ");

	if (request_parts.size() < 2) {
		std::cout << "Cannot parse request header." << std::endl;
		return nullptr;
	}

	auto request = std::make_shared<Request>();
	auto url = Utils::url_decode(request_parts.at(1));

	auto query_position = url.find("?");

	if (query_position != std::string::npos) {
		auto parts = Utils::split(url, "?");

		if (parts[1].find("&") != std::string::npos) {
			auto options = Utils::split(parts[1], "&");

			for (auto& option : options) {
				if (option.find("=") != std::string::npos) {
					auto pair = Utils::split(option, "=");
					request->query.insert({ Utils::trim(pair[0]), Utils::url_decode(Utils::trim(pair[1])) });
				}
			}
		}
		else if (parts[1].find("=") != std::string::npos) {
			auto option = Utils::split(parts[1], "=");
			request->query.insert({ Utils::trim(option[0]), Utils::url_decode(Utils::trim(option[1])) });
		}

		url = url.substr(0, query_position);
	}

	if (url.at(url.size() - 1) == '/' && url.size() > 1)
		url = url.substr(0, url.size() - 1);

	request->url = url;
	request->method = request_parts.at(0);
	request->protocol = request_parts.at(2);

	for (int i = 1; i < lines.size(); ++i)
	{
		auto header = lines.at(i);

		header = Utils::strip(header, '\r');
		header = Utils::strip(header, '\n');

		auto pos = header.find(":");

		if (pos != std::string::npos)
		{
			std::string key = header.substr(0, pos);
			std::string value = header.substr(pos + 2, header.size());
			request->headers[key] = value;
		}
	}

	if (request->method == "POST" || request->method == "PATCH") {
		auto args = Utils::split(parts[1], "&");

		for (auto& arg : args) {
			if (arg.find("=") != std::string::npos) {
				auto pair = Utils::split(arg, "=");
				request->body.insert({ Utils::trim(pair[0]), Utils::url_decode(Utils::trim(pair[1])) });
			}
		}
	}

	return request;
}
