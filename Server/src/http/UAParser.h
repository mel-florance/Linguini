#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <regex>

#include "../orm/ORM.h"
#include "../core/Json.h"

using json = nlohmann::json;

class UAParser {
public:
	UAParser(const std::string& filename) :
		filename(filename) {};

	inline std::string parse(const std::string& user_agent) {
		std::ifstream file(filename);

		if (!file.is_open())
			ORM::logger.error("HTTP", "Cannot open UAParser file %s", filename.c_str());

		std::string str{
			(std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>()
		};

		auto data = json::parse(str);

		for (auto& regex : data["user_agent_parsers"]) {
			auto replacement = regex["replacement"].is_string() ? regex["replacement"].get<std::string>() : "";

			std::smatch matches;
			if (std::regex_search(user_agent, matches, std::regex(regex["regex"].get<std::string>())))
			{
				if (matches.size() > 0)
					return matches[0].str();
			}
		}

		return std::string();
	}

	std::string filename;
};
