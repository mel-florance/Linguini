#pragma once

#include "../orm/ORM.h"

struct Model {
	Model() : 
		schema({}),
		name("Model")
	{}

	virtual ~Model() = default;

	inline Model* findOne(const Object& parameters)
	{
		auto table = Utils::upper(this->name);

		auto query = QueryBuilder::make()
			.select()
			.from(table, std::string(1, table.at(0)));

		if (parameters.size() > 0)
			query.where(parameters);

		query.limit(0, 1);

		auto results = ORM::fetch(this->name, query.getQuery());

		if (results.size() > 0)
			return results.at(0);

		return nullptr;
	}

	inline std::vector<Model*> findAll(const Object& parameters = {}, const std::vector<std::string>& columns = {})
	{
		auto table = Utils::upper(this->name);

		auto query = QueryBuilder::make()
			.select(columns)
			.from(table, std::string(1, table.at(0)))
			.orderBy("id", "ASC");

		if (parameters.size() > 0)
			query.where(parameters);

		query.limit(0, 1000);

		std::vector<Model*> data;

		try {
			auto records = ORM::fetch(this->name, query.getQuery().c_str());

			for (auto record : records)
				data.push_back(record);

			return records;
		}
		catch (std::exception& e) {
			ORM::logger.error("ORM", e.what());
		}

		return std::vector<Model*>();
	}

	inline uint64_t create(const Object& parameters = {})
	{
		auto table_name = Utils::upper(this->name);

		auto query = QueryBuilder::make()
			.insert(table_name, parameters);

		uint64_t inserted = 0;

		try {
			inserted = ORM::execute(this->name, query.getQuery().c_str());
		}
		catch (std::exception& e) {
			ORM::logger.error("ORM", e.what());
		}

		return inserted;
	}

	inline void remove(const Object& parameters = {})
	{
		auto table_name = Utils::upper(this->name);

		auto query = QueryBuilder::make()
			.remove(table_name, parameters);

		try {
			ORM::execute(this->name, query.getQuery().c_str());
		}
		catch (std::exception& e) {
			ORM::logger.error("ORM", e.what());
		}
	}

	inline uint64_t update(const Object& parameters = {})
	{
		auto table_name = Utils::upper(this->name);

		auto query = QueryBuilder::make()
			.update(table_name, parameters);

		uint64_t updated = 0;

		try {
			updated = ORM::execute(this->name, query.getQuery().c_str());
		}
		catch (std::exception& e) {
			ORM::logger.error("ORM", e.what());
		}

		return updated;
	}

	std::string name;
	std::vector<QueryBuilder::Field> schema;
};
