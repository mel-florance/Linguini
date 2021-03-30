#pragma once

#include <string>
#include <iostream>
#include <unordered_map>
#include <optional>

#include "TCPListener.h"
#include "Headers.h"
#include "Route.h"
#include "../core/Utils.h"

class WebServer;

struct Request {
	Request(const std::string& payload) :
		client(nullptr),
		payload(payload),
		headers(Headers()),
		server(nullptr),
		params({}),
		body({}),
		query({}),
		credentials()
	{}

	inline void parse()
	{
		auto parts = Utils::split(payload, "\r\n\r\n");

		if (parts.size() <= 0) {
			std::cout << "Cannot parse http payload." << std::endl;
			return;
		}

		auto lines = Utils::split(parts[0], "\r\n");

		if (lines.size() <= 0) {
			std::cout << "Cannot parse http headers." << std::endl;
			return;
		}

		auto request_parts = Utils::split(lines.at(0), " ");

		if (request_parts.size() < 2) {
			std::cout << "Cannot parse request header." << std::endl;
			return;
		}

		auto url = Utils::url_decode(request_parts.at(1));
		auto query_position = url.find("?");

		if (query_position != std::string::npos) {
			auto parts = Utils::split(url, "?");

			if (parts[1].find("&") != std::string::npos) {
				auto options = Utils::split(parts[1], "&");

				for (auto& option : options) {
					if (option.find("=") != std::string::npos) {
						auto pair = Utils::split(option, "=");
						query.insert({ Utils::trim(pair[0]), Utils::url_decode(Utils::trim(pair[1])) });
					}
				}
			}
			else if (parts[1].find("=") != std::string::npos) {
				auto option = Utils::split(parts[1], "=");
				query.insert({ Utils::trim(option[0]), Utils::url_decode(Utils::trim(option[1])) });
			}

			url = url.substr(0, query_position);
		}

		if (url.at(url.size() - 1) == '/' && url.size() > 1)
			url = url.substr(0, url.size() - 1);

		headers.url = url;
		headers.method = request_parts.at(0);
		headers.protocol = request_parts.at(2);

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
				headers.entries[key] = value;
			}
		}

		if (headers.method == "POST" || headers.method == "PATCH") {
			auto args = Utils::split(parts[1], "&");

			for (auto& arg : args) {
				if (arg.find("=") != std::string::npos) {
					auto pair = Utils::split(arg, "=");
					body.insert({ Utils::trim(pair[0]), Utils::url_decode(Utils::trim(pair[1])) });
				}
			}
		}

		headers.parse(server, client);
	}

	inline std::string getParam(const std::string& key) {
		auto it = params.find(key);

		if (it != params.end())
			return it->second;

		return std::string();
	}

	inline std::string getBody(const std::string& key) {
		auto it = body.find(key);

		if (it != body.end())
			return it->second.c_str();

		return std::string();
	}

	inline std::string getQuery(const std::string& key) {
		auto it = query.find(key);

		if (it != query.end())
			return it->second;

		return std::string();
	}

	TCPListener::TCPSocket* client;
	Route route;
	std::string payload;
	Headers headers;
	WebServer* server;
	std::unordered_map<std::string, std::string> params;
	std::unordered_map<std::string, std::string> body;
	std::unordered_map<std::string, std::string> query;
	std::optional<std::pair<std::string, std::string>> credentials;
};
