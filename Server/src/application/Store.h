#pragma once

#include <unordered_map>
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <filesystem>
#include <vector>

#include "../core/Utils.h"
#include "../core/Json.h"
#include "../application/Model.h"
#include "../http/Route.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

class Store
{
public:
	Store() {}

	struct SecurityDefinition {
		std::string name;
		std::string type;
		std::string url;
		std::string flow;
		std::unordered_map<std::string, std::pair<std::string, std::string>> scopes;
	};

	void parse(const std::string& directory);

	void printDebug() {

		auto table = ORM::createDebugTable();
		table[0][0] = "Title";
		table[0][1] = title;
		table[1][0] = "Description";
		table[1][1] = description;
		table[2][0] = "Terms of service";
		table[2][1] = terms_of_service;
		table[3][0] = "Contact email";
		table[3][1] = contact_email;
		table[4][0] = "License name";
		table[4][1] = license_name;
		table[5][0] = "License url";
		table[5][1] = license_url;
		table[6][0] = "Host";
		table[6][1] = host;
		table[7][0] = "Base path";
		table[7][1] = basePath;
		table[8][0] = "Schemes";
		table[8][1] = Utils::join(schemes, ", ");

		std::cout << table;
	}

	std::vector<Route> http_routes;
	std::vector<Model> orm_definitions;
private:
	std::string title;
	std::string version;
	std::string description;
	std::string terms_of_service;
	std::string contact_email;
	std::string license_name;
	std::string license_url;
	std::string host;
	std::string basePath;
	std::vector<std::string> schemes;
	std::vector<SecurityDefinition> security_definitions;
};
