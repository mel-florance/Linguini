#pragma once

#include <unordered_map>
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <filesystem>

#include "../core/Utils.h"
#include "../core/Json.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

class Store
{
public:
	Store(){}

	void parse(const std::string& directory) {

		for (const auto& entry : fs::directory_iterator(directory))
		{
			if (fs::path(entry.path()).extension() == std::string_view(".json"))
			{
				std::fstream file(entry.path(), std::fstream::in);
				std::string str{
					(std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>()
				};

				try {
					auto values = json::parse(str.c_str());

					for (auto& value : values) {
						std::cout << "Getting  value" << std::endl;
					}
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}
			}
		}
	}
};
