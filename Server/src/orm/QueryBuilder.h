#pragma once

#include "../core/Types.h"
#include "../core/Utils.h"

struct Model;
class QueryBuilder
{
public:
	QueryBuilder() :
		columns({}),
		table(""),
		table_alias(""),
		selects({}),
		conditions({}),
		offset_min(-1),
		offset_max(-1),
		operation(QueryOperation::NONE)
	{
		sql_functions = {
			"NOW",
			"GREATEST"
		};
	}

	struct Field 
	{
		enum Type {
			INTEGER,
			STRING,
			FLOAT,
			DATETIME,
			CHAR,
			TEXT
		};

		Field(
			const std::string& name = "",
			Type type = Type::INTEGER,
			const std::any& value = 0,
			int size = 0,
			bool not_null = false,
			bool primary = false,
			bool increment = false
		) : 
			name(name),
			type(type),
			value(value),
			size(size),
			not_null(not_null),
			primary(primary),
			increment(increment)
		{}

		std::string name;
		Type type;
		std::any value;
		int size;
		bool not_null;
		bool primary;
		bool increment;
	};

	inline static std::string fieldTypeToStr(Field::Type type) {
		switch (type) {
		case Field::Type::CHAR: return "char";
		case Field::Type::DATETIME: return "datetime";
		case Field::Type::FLOAT: return "float";
		case Field::Type::INTEGER: return "integer";
		case Field::Type::STRING: return "varchar";
		case Field::Type::TEXT: return "text";
		}
	}

	enum class QueryOperation {
		NONE,
		SELECT,
		INSERT,
		UPDATE,
		REMOVE,
		TRUNCATE
	};

	static QueryBuilder make() {
		return QueryBuilder();
	}

	QueryBuilder& select(
		const std::vector<std::string>& columns = {}
	) {
		this->selects = columns;
		this->operation = QueryOperation::SELECT;

		return *this;
	}

	QueryBuilder& insert(
		const std::string& table,
		const Object& columns = {})
	{
		this->table = table;
		this->columns = columns;
		this->operation = QueryOperation::INSERT;

		return *this;
	}

	QueryBuilder& update(
		const std::string& table, 
		const Object& columns = {}) 
	{
		this->table = table;
		//this->selects = columns;
		this->operation = QueryOperation::UPDATE;

		return *this;
	}

	QueryBuilder& remove(
		const std::string& table, 
		const Object& parameters = {}
	) {
		this->table = table;
		this->operation = QueryOperation::REMOVE;
		this->table_alias = Utils::lower(table).at(0);

		auto params = parametersToString(parameters, false, "AND");

		if (params.size() > 0)
			this->conditions.push_back(params);

		return *this;
	}

	QueryBuilder& truncate(const std::string& table) {
		this->table = table;
		this->operation = QueryOperation::TRUNCATE;

		return *this;
	}

	QueryBuilder& from(
		const std::string& table, 
		const std::string& alias = ""
	) {
		this->table = table;
		this->table_alias = Utils::lower(alias);

		return *this;
	}

	QueryBuilder& innerJoin(
		const std::string& table,
		const std::vector<std::string>& condition
	) {
		this->joins.push_back(std::make_pair(table, condition));

		return *this;
	}

	QueryBuilder& leftJoin() {
		return *this;
	}

	QueryBuilder& where(const Object& parameters) {
		auto params = parametersToString(parameters, false, "AND");
		this->conditions.push_back(params);

		return *this;
	}

	QueryBuilder& andWhere(const Object& parameters) {
		auto params = parametersToString(parameters, false, "AND");
		this->conditions.push_back(params);

		return *this;
	}

	QueryBuilder& orWhere(const Object& parameters) {
		auto params = parametersToString(parameters, false, "OR");
		this->conditions.push_back(params);

		return *this;
	}

	QueryBuilder& groupBy(const std::string& column) {
		this->group_by = column;
		return *this;
	}

	QueryBuilder& addGroupBy() {
		return *this;
	}

	QueryBuilder& having() {
		return *this;
	}

	QueryBuilder& andHaving() {
		return *this;
	}

	QueryBuilder& orHaving() {
		return *this;
	}

	QueryBuilder& orderBy(
		const std::string& column, 
		const std::string& direction = "ASC")
	{
		auto alias = Utils::lower(std::string(1, this->table.at(0)));
		this->orders.push_back(alias + ".`" + column + "` " + direction);

		return *this;
	}

	QueryBuilder& limit(int min = 0, int max = 1) {
		this->offset_min = min;
		this->offset_max = max;

		return *this;
	}

	static std::string createTable(Model* model);

	std::string getQuery() {
		std::string sql;
		std::vector<std::string> statements;

		std::string columns;
		std::string operation;

		// OPERATION
		switch (this->operation)
		{
			case QueryOperation::SELECT: {
				columns = this->selects.size() > 0
					? Utils::join(this->selects, ", ")
					: "*";

				statements.push_back(Utils::join({
					"SELECT", columns 
				}, " "));
				break;
			}
			case QueryOperation::INSERT: {
				columns = Utils::join(this->columnsToString(), ", ");
				statements.push_back(Utils::join({
					"INSERT INTO", this->table, 
					"SET", columns 
				}, " "));
				break;
			}
			case QueryOperation::UPDATE: {
				columns = Utils::join(this->columnsToString(), ", ");
				statements.push_back(Utils::join({
					"UPDATE", this->table,
					"SET", columns
					}, " "));
				break;
			}
			case QueryOperation::REMOVE: {
				statements.push_back("DELETE");
				break;
			}
			case QueryOperation::TRUNCATE: {
				statements.push_back(Utils::join({
					"TRUNCATE", this->table
					}, " "));
				break;
			}
		}

		// FROM
		if (this->table.size() > 0
			&& this->operation != QueryOperation::INSERT
			&& this->operation != QueryOperation::UPDATE)
		{
			statements.push_back(Utils::join({
				"FROM", this->table, this->table_alias
			}, " "));
		}
			
		// JOIN
		if (this->joins.size() > 0)
		{
			for (auto join : this->joins) 
			{
				auto table = std::get<0>(join);
				auto condition = Utils::join(std::get<1>(join), " AND ");

				statements.push_back(Utils::join({ 
					"JOIN", table, 
					"ON", condition
				}));
			}
		}

		// WHERE
		if (this->conditions.size() > 0)
			statements.push_back("WHERE " + Utils::join(this->conditions, " AND "));

		//GROUP BY
		if (this->group_by.size() > 0)
			statements.push_back("GROUP BY " + this->group_by);

		// ORDER BY
		if (this->orders.size() > 0)
			statements.push_back("ORDER BY " + Utils::join(this->orders, ", "));

		// LIMIT
		if (this->offset_min != -1 && this->offset_max != -1) 
		{
			statements.push_back("LIMIT " + Utils::join({
				std::to_string(this->offset_min),
				std::to_string(this->offset_max)
			}, ", "));
		}

		return Utils::join(statements, " ") + ';';
	}

	std::vector<std::string> columnsToString();

	std::string parametersToString(
		const Object& parameters,
		bool selector,
		const std::string& delimiter
	);

	std::string table;
	std::string table_alias;

private:
	Object columns;
	QueryOperation operation;
	std::vector<std::string> selects;
	std::vector<std::string> conditions;
	std::vector<std::pair<std::string, std::vector<std::string>>> joins;
	std::vector<std::string> orders;
	std::string group_by;
	int offset_min;
	int offset_max;
	std::vector<std::string> sql_functions;
};
