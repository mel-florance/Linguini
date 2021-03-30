#include "QueryBuilder.h"
#include "../application/Model.h"

std::string QueryBuilder::createTable(Model* model)
{
	std::string str = "CREATE TABLE IF NOT EXISTS " + Utils::upper(model->name) + " (\n";
	std::vector<std::string> fields;

	for (auto field : model->schema)
	{
		std::string out = "  `" + field.name + '`';

		switch (field.type) {
		case Field::Type::INTEGER: {
			out += " INT(" + std::to_string(field.size > 0 ? field.size : 11) + ")";
			break;
		}
		case Field::Type::STRING: {
			out += " VARCHAR(" + std::to_string(field.size > 0 ? field.size : 255) + ")";
			break;
		}
		case Field::Type::DATETIME: {
			out += " DATETIME";
			break;
		}
		case Field::Type::CHAR: {
			out += " CHAR";
			break;
		}
		case Field::Type::FLOAT: {
			out += " FLOAT";
			break;
		}
		case Field::Type::TEXT: {
			out += " TEXT";
			break;
		}
		}

		if (field.primary)
			out += " PRIMARY KEY";

		if (field.not_null)
			out += " NOT NULL";
		else
			out += " DEFAULT NULL";

		if (field.increment)
			out += " AUTO_INCREMENT";

		fields.push_back(out);
	}

	str += Utils::join(fields, ",\n");
	str += "\n);";

	try {
		ORM::fetch(model->name, str.c_str());
	}
	catch (std::exception& e) {
		ORM::logger.error("ORM", e.what());
	}

	return std::string();
}

std::vector<std::string> QueryBuilder::columnsToString()
{
	std::vector<std::string> lines;

	for (auto it = this->columns.begin(); it != this->columns.end(); ++it) {
		std::string value = ORM::getAnyValue(it->second);

		for (auto function : this->sql_functions)
			if (value.find(function + '(') != std::string::npos)
				value = value.substr(1, value.size() - 2);

		lines.push_back(Utils::join({ '`' + it->first + '`', value }, " = "));
	}

	return lines;
}

std::string QueryBuilder::parametersToString(const Object& parameters, bool selector, const std::string& delimiter)
{
	std::stringstream stream;
	unsigned int count = 0;
	auto it = parameters.begin();

	for (; it != parameters.end(); ++it) {

		stream << "`" + this->table_alias + "`.`" + it->first + "`";

		if (!selector)
			stream << " = " << ORM::getAnyValue(it->second);

		if (count >= 0 && count < parameters.size() - 1)
			stream << ' ' + delimiter + ' ';

		count++;
	}

	return stream.str();
}
