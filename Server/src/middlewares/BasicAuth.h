#pragma once

#include "../application/Middleware.h"

class BasicAuth : public Middleware {
public:
	BasicAuth() : Middleware() {}

	bool use(Request& request, Response& response) override
	{
		bool authorized = false;

		for (auto& header : request.headers.entries)
		{
			if (header.first == "Authorization") {
				auto parts = Utils::split(header.second, " ");

				if (parts.size() > 0) {
					if (parts[0] == "Basic") {
						auto decoded = Utils::base64_decode(parts[1]);

						if (decoded.find(":") != std::string::npos) {
							auto auth = Utils::split(decoded, ":");

							if (auth.size() > 0) {
								std::pair<std::string, std::string> credentials = {
									Utils::trim(auth[0]),
									Utils::trim(auth[1])
								};

								if (credentials.first == "Mel" && credentials.second == "salut") {
									authorized = true;
									//std::cout << credentials.first << " -> " << credentials.second << std::endl;
								}
								else {
									response.body = "Forbidden";
									response.status = StatusCode::FORBIDDEN;
								}
							}
						}
					}
				}
			}
		}

		if (!authorized) {
			response.setHeader("WWW-authenticate", "Basic realm=\"Authorization required !\"");
		}

		return authorized;
	}
};
