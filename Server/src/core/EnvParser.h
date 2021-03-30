#pragma once

#include <unordered_map>
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <filesystem>

#include "Utils.h"
#include "Json.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

const static std::array<std::string, 3> SENSITIVES_KEYS = {
	"password",
	"key",
	"certificate"
};

class EnvParser
{
public:
	typedef std::pair<std::string, int> VariableOffset;

	EnvParser(const std::string& filename = "") : filename(filename)
	{
		if (filename.size() == 0) {
			std::cout << "EnvFileParser: Invalid filename.\n";
			return;
		}

		std::ifstream file(filename);

		if (!file.good()) {
			std::cout << "EnvFileParser: Cannot open file.\n";
			return;
		}

		std::string line;

		while (std::getline(file, line))
		{
			if (line.find('#') == 0 || line.size() == 0)
				continue;

			auto position = line.find('=');
			const auto key = line.substr(0, position);

			std::string value;

			if (line.at(position + 1) == '"'
			&& line.at(line.size() - 1) == '"')
				value = line.substr(position + 2, line.size() - position - 3);
			else
				value = line.substr(position + 1, line.size() - 1);

			set(key, value);
			std::vector<VariableOffset> replacements;

			for (auto& variable : variables) {
				auto offset = value.find('$' + variable.first);
			
				if (offset != std::string::npos)
					replacements.push_back({ variable.first, offset });
			}

			for (auto& needle : replacements) 
			{
				auto variable = variables.find(needle.first);

				if (variable != variables.end()) {
					auto index = 1;

					while ((index = value.find(needle.first, index)) != std::string::npos) {
						value.replace(index, needle.first.length(), variable->second);
						index += variable->second.length();
					}

					value.erase(std::remove(value.begin(), value.end(), '$'), value.end());
					set(key, value);
				}
			}
		}
	}

	inline void set(const std::string& key, const std::string& value) {
		variables[key] = value;
	}

	inline std::string get(const std::string& name) {
		auto result = variables.find(name);

		if (result != variables.end())
			return result->second.c_str();

		return "";
	}

	inline json toJson() {
		json data = {};

		auto it = variables.begin();
		auto mask = std::string(10, '*');

		for (int row = 0; it != variables.end(); ++it, ++row)
		{
			json object;
			auto needle = Utils::lower(it->first);
			auto value = it->second;

			for (auto key : SENSITIVES_KEYS)
				if (needle.find(key) != std::string::npos)
					value = mask;

			object["key"] = it->first;
			object["value"] = value;

			data.push_back(object);
		}

		return data;
	};

	template<typename T>
	inline void print(T& table) {
		table[0][0] = filename;

		auto it = variables.begin();
		auto mask = std::string(10, '*');

		for (int row = 0; it != variables.end(); ++it, ++row) 
		{
			auto needle = Utils::lower(it->first);

			for (auto key : SENSITIVES_KEYS)
				if (needle.find(key) != std::string::npos)
					it->second = mask;

			table[row + 1][0] = it->first;
			table[row + 1][1] = it->second;
		}

		std::cout << table;
	}

private:
	std::string filename;
	std::map<std::string, std::string> variables;
};
