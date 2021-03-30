#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <sstream>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <memory_resource>

#include <Windows.h>
#include <mysql.h>

#include "QueryBuilder.h"
#include "../core/ConsoleTable.h"
#include "../core/EnvParser.h"
#include "../core/Logger.h"
#include "../core/Types.h"
#include "../core/Utils.h"

class Model;
class WebServer;
using ConsoleTable = samilton::ConsoleTable;

class ORM
{
public:
	ORM();
	ORM(
		const std::string& host,
		const std::string& user,
		const std::string& password,
		const std::string& database,
		unsigned short int port = 3306
	);
	~ORM();

	bool connect();

	static uint64_t execute(const std::string& name, const std::string& query);
	static std::vector<Model*> fetch(const std::string& name, const std::string& query);

	static std::string toJson(const std::vector<Model*>& records);
	static Model* getModel(const std::string& name);
	static void registerModel(Model* model);
	static void loadModels(const std::string& path);
	static void printResults(const std::vector<Model*>& records);
	static void printResults(Model* model);
	static std::string getAnyValue(const std::any& value);
	static ConsoleTable createDebugTable(int paddingLeft = 1, int paddingTop = 1);
	void scanDatabase();


	static EnvParser env;
	static MYSQL* mysql;
	static Logger logger;
	static std::map<std::string, Model*> models;
	static bool debug;
	static bool log_queries;

	bool useEnvFile;
	bool connected;
	std::string host;
	std::string user;
	std::string password;
	std::string database;
	unsigned short int port;
};

