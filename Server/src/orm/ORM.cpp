#include "ORM.h"
#include "../application/Model.h"
#include "../core/Json.h"

#include <filesystem>

namespace fs = std::filesystem;
using json = nlohmann::json;

EnvParser ORM::env = EnvParser("./project/config/dev.env");
MYSQL* ORM::mysql = nullptr;
bool ORM::debug = true;
bool ORM::log_queries = false;
Logger ORM::logger = Logger();
std::map<std::string, Model*> ORM::models = {};

ORM::ORM() :
	connected(false),
	useEnvFile(true)
{
	this->connect();
}

ORM::ORM(
	const std::string& host, 
	const std::string& user,
	const std::string& password,
	const std::string& database,
	unsigned short int port
) :
	useEnvFile(false),
	connected(false),
	host(host),
	user(user),
	password(password),
	database(database),
	port(port)
{
	this->connect();
}

ORM::~ORM() {
}

bool ORM::connect()
{
	mysql = mysql_init(nullptr);

	connected = false;
	debug = env.get("DEBUG").compare("false") ? true : false;

	try
	{
		if (useEnvFile) {
			host = env.get("DB_HOST");
			user = env.get("DB_USER");
			password = env.get("DB_PASSWORD");
			database = env.get("DB_NAME");
			port = std::atoi(env.get("DB_PORT").c_str());
			log_queries = env.get("DB_LOG_QUERIES") == "true";
		}

		connected = mysql_real_connect(
			mysql,
			host.c_str(),
			user.c_str(),
			password.c_str(),
			database.c_str(),
			port,
			NULL,
			0
		);

		//scanDatabase();
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	if (!connected) {
		logger.error("ORM", "Cannot connect to the database.");
		std::wcout << mysql_error(mysql) << std::endl;
		return false;
	}

	logger.success("ORM", "Database connection established using Mysql %s", mysql->server_version);

	return true;
}

std::string ORM::toJson(const std::vector<Model*>& records)
{
	json data = {};

	for (auto record : records)
	{
		if (!record)
			continue;

		json object;

		for (auto& field : record->schema) {
			if (field.value.type() == typeid(int))
				object[field.name] = std::any_cast<int>(field.value);
			else if (field.value.type() == typeid(char const*))
				object[field.name] = std::any_cast<char const*>(field.value);
			else if (field.value.type() == typeid(std::string))
				object[field.name] = std::any_cast<std::string>(field.value);
			else if (field.value.type() == typeid(float))
				object[field.name] = std::any_cast<float>(field.value);
			else if (field.value.type() == typeid(char))
				object[field.name] = std::any_cast<char>(field.value);
			else if (field.value.type() == typeid(Time)) {
				auto datetime = std::any_cast<Time>(field.value);
				object[field.name] =
					Utils::pad(datetime.year, 3) + '-' +
					Utils::pad(datetime.month) + '-' +
					Utils::pad(datetime.day) + ' ' +
					Utils::pad(datetime.hour) + ':' +
					Utils::pad(datetime.minute) + ':' +
					Utils::pad(datetime.second);
			}
		}

		if (records.size() == 1) {
			return object.dump(4);
		}

		delete record;
		data.push_back(object);
	}

	return data.dump(4);
}

Model* ORM::getModel(const std::string& name)
{
	auto it = models.find(name);

	if (it != models.end())
		return it->second;

	return nullptr;
};

uint64_t ORM::execute(const std::string& name, const std::string& query) {
	if (log_queries)
		logger.info("ORM", query.c_str());

	mysql_query(mysql, query.c_str());

	uint64_t insert_id = 0;

	if (query.find("INSERT INTO") != std::string::npos
	|| query.find("UPDATE") != std::string::npos) {
		insert_id = mysql_insert_id(mysql);
	}

	return insert_id;
}

std::vector<Model*> ORM::fetch(const std::string& name, const std::string& query)
{
	if (log_queries)
		logger.info("ORM", query.c_str());

	mysql_query(mysql, query.c_str());

	std::vector<Model*> data = {};

	if (fetch)
	{
		MYSQL_RES* result = mysql_store_result(mysql);

		if (result)
		{
			int rows = mysql_num_rows(result);
			int fields = mysql_num_fields(result);
			MYSQL_ROW row;

			auto table_name = Utils::lower(name);
			auto mdl = ORM::getModel(Utils::upper(std::string(1, table_name.at(0))) + table_name.substr(1, table_name.size()));

			while ((row = mysql_fetch_row(result)))
			{
				Model* model = new Model();
				model->schema = { mdl->schema };

				for (int i = 0; i < fields; i++)
				{
					if (model->schema[i].value.type() == typeid(int))
						model->schema[i].value = std::stoi(row[i]);
					else if (model->schema[i].value.type() == typeid(float))
						model->schema[i].value = std::stof(row[i]);
					else if (model->schema[i].value.type() == typeid(char))
						model->schema[i].value = std::string(row[i]);
					else if (model->schema[i].value.type() == typeid(char const*))
						model->schema[i].value = std::string(row[i]);
					else if (model->schema[i].value.type() == typeid(std::string))
						model->schema[i].value = std::string(row[i]);
					else if (model->schema[i].value.type() == typeid(Time))
					{
						Time datetime;
						auto parts = Utils::split(std::string(row[i]), " ");
						auto date = Utils::split(std::string(parts[0]), "-");
						auto time = Utils::split(std::string(parts[1]), ":");

						datetime.year = std::atoi(date[0].c_str());
						datetime.month = std::atoi(date[1].c_str());
						datetime.day = std::atoi(date[2].c_str());
						datetime.hour = std::atoi(time[0].c_str());
						datetime.minute = std::atoi(time[1].c_str());
						datetime.second = std::atoi(time[2].c_str());

						model->schema[i].value = datetime;
					}
				}

				data.push_back(model);
			}

			mysql_free_result(result);
		}
	}

	return data;
}

void ORM::registerModel(Model* model)
{
	ORM::models.insert(std::make_pair(model->name, model));
	logger.info("ORM", "Registered model %s", model->name.c_str());
	QueryBuilder::createTable(model);
};

void ORM::loadModels(const std::string& path)
{
	for (const auto& entry : fs::directory_iterator(path))
	{
		if (fs::path(entry.path()).extension() == std::string_view(".json"))
		{
			std::fstream file(entry.path(), std::fstream::in);
			std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

			auto result = json::parse(str);
			auto fields = result["fields"];
			
			auto model = new Model();
			model->name = result["name"].get<std::string>();

			for (auto& value : fields) {
				QueryBuilder::Field field;

				field.name = value["name"].get<std::string>();
				auto type = value["type"].get<std::string>();

				if (type == "integer") {
					field.type = QueryBuilder::Field::Type::INTEGER;
					field.value = 0;
				}
				else if (type == "string") {
					field.type = QueryBuilder::Field::Type::STRING;
					field.value = std::string();
				}
				else if (type == "float") {
					field.type = QueryBuilder::Field::Type::FLOAT;
					field.value = 0.0f;
				}
				else if (type == "datetime") {
					field.type = QueryBuilder::Field::Type::DATETIME;
					field.value = Time();
				}
				else if (type == "char") {
					field.type = QueryBuilder::Field::Type::CHAR;
					field.value = std::string();
				}
				else if (type == "text") {
					field.type = QueryBuilder::Field::Type::TEXT;
					field.value = std::string();
				}

				try {
					if (value["not_null"].is_boolean())
						field.not_null = value["not_null"].get<bool>();
					if (value["primary"].is_boolean())
						field.primary = value["primary"].get<bool>();
					if (value["increment"].is_boolean())
						field.increment = value["increment"].get<bool>();
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}

				model->schema.push_back(field);
			}

			registerModel(model);
		}
	}
}

void ORM::printResults(Model* model)
{
	printResults(std::vector<Model*>({ model }));
}

void ORM::printResults(const std::vector<Model*>& records)
{
	auto table = createDebugTable();

	if (records.size() > 0) {
		int col = 0;
		auto first = records.at(0);
		auto i = first->schema.begin();

		for (; i != first->schema.end(); i++, col++)
			table[0][col] = i->name;

		for (size_t row = 0; row < records.size(); ++row) {
			col = 0;
			auto record = records.at(row);
			auto j = record->schema.begin();

			for (; j != record->schema.end(); j++, col++)
				table[row + 1][col] = getAnyValue(j->value);
		}
	}

	std::cout << table << '\n';
}

std::string ORM::getAnyValue(const std::any& value)
{
	std::string out;

	try {
		if (value.type() == typeid(int))
			out = std::to_string(std::any_cast<int>(value));
		else if (value.type() == typeid(char const*))
			out = '"' + Utils::escape(std::string(std::any_cast<char const*>(value))) + '"';
		else if (value.type() == typeid(std::string))
			out = '"' + Utils::escape(std::any_cast<std::string>(value)) + '"';
		else if (value.type() == typeid(float))
			out = std::to_string(std::any_cast<float>(value));
		else if (value.type() == typeid(Time))
		{
			auto datetime = std::any_cast<Time>(value);

			out =
				Utils::pad(datetime.year, 3) + '-' +
				Utils::pad(datetime.month) + '-' +
				Utils::pad(datetime.day) + ' ' +
				Utils::pad(datetime.hour) + ':' +
				Utils::pad(datetime.minute) + ':' +
				Utils::pad(datetime.second);
		}
		else if (value.type() == typeid(char))
			out = Utils::escape(std::string(1, std::any_cast<char>(value)));
	}
	catch (const std::bad_any_cast& e) {
		logger.error("ORM", e.what());
	}

	return out;
}

ConsoleTable ORM::createDebugTable(int paddingLeft, int paddingTop)
{
	ConsoleTable table(1, 1);
	ConsoleTable::TableChars chars;

	chars.topLeft = '+';
	chars.topRight = '+';
	chars.downLeft = '+';
	chars.downRight = '+';
	chars.topDownSimple = '-';
	chars.leftRightSimple = '|';
	chars.leftSeparation = '+';
	chars.rightSeparation = '+';
	chars.centreSeparation = '+';
	chars.topSeparation = '+';
	chars.downSeparation = '+';

	table.setTableChars(chars);

	return table;
}

void ORM::scanDatabase()
{
	logger.info("ORM", "Scanning database...");

	auto query =
		"SELECT *"
		" FROM INFORMATION_SCHEMA.COLUMNS"
		" WHERE TABLE_SCHEMA = 'warship'"
		" ORDER BY ORDINAL_POSITION ASC;";

	mysql_query(mysql, query);

	MYSQL_RES* result = mysql_store_result(mysql);

	if (result)
	{
		int n = mysql_num_rows(result);
		int fields = mysql_num_fields(result);

		MYSQL_ROW row;
		std::map<std::string, std::vector<Object>> data;
		std::vector<Model> models;

		while (row = mysql_fetch_row(result)) {
			data[std::string(row[2])].push_back({
				{"schema", std::string(row[1]) },
				{"table", std::string(row[2]) },
				{"column", std::string(row[3]) },
				{"order",  std::string(row[4]) },
				{"nullable", std::string(row[6]) },
				{"type", std::string(row[7]) },
				{"primary", std::string(row[16]) },
				{"increment", std::string(row[17]) },
				{"privileges", std::string(row[18]) }
			});
		}

		mysql_free_result(result);

		for (auto& m : data)
		{
			Model model;
			model.name = m.first;

			for (auto& f : m.second)
			{
				QueryBuilder::Field field;
				field.name = Utils::strip(getAnyValue(f["column"]));
				field.increment = Utils::strip(getAnyValue(f["increment"])) == "auto_increment";
				field.not_null = Utils::strip(getAnyValue(f["nullable"])) == "YES";
				field.primary = Utils::strip(getAnyValue(f["primary"])) == "PRI";

				auto type = getAnyValue(f["type"]);

				if (type == "varchar")
					field.type = QueryBuilder::Field::Type::STRING;
				else if (type == "datetime")
					field.type = QueryBuilder::Field::Type::DATETIME;
				else if (type == "float")
					field.type = QueryBuilder::Field::Type::FLOAT;
				else if (type == "int")
					field.type = QueryBuilder::Field::Type::INTEGER;
				else if (type == "char")
					field.type = QueryBuilder::Field::Type::CHAR;
				else if (type == "text")
					field.type = QueryBuilder::Field::Type::TEXT;

				model.schema.push_back(field);
			}

			models.push_back(model);
		}

		for (auto& model : models) {
			registerModel(&model);
		}
	}
}
