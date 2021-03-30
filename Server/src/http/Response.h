#pragma once

#include <string>
#include "Headers.h"
#include "../core/Inja.h"

using namespace inja;
using json = nlohmann::json;

struct Response
{
	Response(
		const std::string& body,
		StatusCode status = StatusCode::OK,
		const std::string& contentType = "text/plain"
	) :
		binary(false),
		data(nullptr),
		size(0),
		status(status),
		headers(Headers()),
		body(body),
		contentType(contentType)
	{ };

	void setHeader(const std::string& name, const std::string& value) {
		headers.entries[name] = value;
	}

	std::string toString()
	{
		auto version = "HTTP/1.1";
		auto statusCode = std::to_string(int(status));
		auto statusText = std::string(statusCodeToStr(int(status)));

		std::ostringstream payload;
		payload << Utils::join({ version, statusCode, statusText }, " ") + "\r\n";

		setHeader("Cache-Control", "no-cache, private");
		setHeader("Content-Length", std::to_string(body.length()));
		setHeader("Content-Type", contentType);
		setHeader("X-Content-Type-Options", "nosniff");
		setHeader("Server", "OHMyServer");

		auto it = headers.entries.begin();
		for (; it != headers.entries.end(); ++it)
			payload << it->first + ": " + it->second + "\r\n";

		payload << "\r\n";

		if (!binary)
			payload << body;

		return payload.str();
	}

	inline static Response render(const std::string& viewName, const json& data) {
		Response response("");
		Environment env;

		try {
			Template tpl = env.parse_template("./project/views/" + viewName + ".html");
			response.body = env.render(tpl, data);
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}

		response.status = StatusCode::OK;
		response.contentType = "text/html; charset=\"UTF-8\"";

		return response;
	}

	bool binary;
	char* data;
	int size;
	StatusCode status;
	Headers headers;
	std::string body;
	std::string contentType;
};
