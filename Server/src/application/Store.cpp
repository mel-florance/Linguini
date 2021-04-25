#include "Store.h"

void Store::parse(const std::string& directory)
{
	for (const auto& entry : fs::directory_iterator(directory))
	{
		if (fs::path(entry.path()).extension() == std::string_view(".json"))
		{
			std::fstream file(entry.path(), std::fstream::in);

			std::string str{ (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>() };

			try {
				auto values = json::parse(str.c_str());

				if (values["swagger"].is_string()) {
					auto version = values["swagger"].get<std::string>();

					if (version != "2.0") {
						ORM::logger.error("STORE", "Swagger version is not supported");
						break;
					}

					if (values["info"].is_object()) {
						auto info = values["info"].get<json>();

						if (info["title"].is_string())
							this->title = info["title"].get<std::string>();
						if (info["description"].is_string())
							this->description = info["description"].get<std::string>();
						if (info["version"].is_string())
							this->version = info["version"].get<std::string>();
						if (info["termsOfService"].is_string())
							this->terms_of_service = info["termsOfService"].get<std::string>();

						if (info["contact"].is_object()) {
							auto contact = info["contact"].get<json>();

							if (contact["email"].is_string())
								this->contact_email = contact["email"].get<std::string>();
						}

						if (info["license"].is_object()) {
							auto license = info["license"].get<json>();

							if (license["name"].is_string())
								this->license_name = license["name"].get<std::string>();
							if (license["url"].is_string())
								this->license_url = license["url"].get<std::string>();
						}
					}

					if (values["host"].is_string())
						this->host = values["host"].get<std::string>();

					if (values["basePath"].is_string())
						this->basePath = values["basePath"].get<std::string>();

					if (values["schemes"].is_array()) {
						for (auto& scheme : values["schemes"].get<json>())
							this->schemes.push_back(scheme);
					}

					if (values["paths"].is_object()) {
						auto routes = values["paths"].get<json>();

						for (auto& item : routes.items())
						{
							auto url = std::regex_replace(item.key(), std::regex("\\{([a-zA-Z0-9_\\-]+)\\}"), ":$1");

							if (item.value().is_object()) {
								auto params = item.value().get<json>();

								for (auto& param : params.items()) {
									Route route;
									route.url = url;
									route.method = Utils::upper(param.key());
									route.callback = "Status.routes";
									route.alias = param.key();

									if (param.value().is_object()) {
										auto options = param.value().get<json>();

										for (auto& option : options.items())
										{
											if (option.key() == "summary")
												route.summary = option.value();
											else if (option.key() == "description")
												route.description = option.value();
											else if (option.key() == "operationId")
												route.name = option.value();
											else if (option.key() == "produces") {
												if (option.value().is_array()) {
													auto mime_types = option.value().get<json>();

													for (auto& mime : mime_types)
														route.produces.push_back(mime);
												}
											}
											else if (option.key() == "consumes") {
												if (option.value().is_array()) {
													auto mime_types = option.value().get<json>();

													for (auto& mime : mime_types)
														route.consumes.push_back(mime);
												}
											}
											else if (option.key() == "parameters") {
												if (option.value().is_array()) {
													auto parameters = option.value().get<json>();

													for (auto& parameter : parameters) {
														if (parameter.is_object()) {
															auto parts = parameter.get<json>();

															for (auto& part : parts.items()) {
																if (part.key() == "in") {

																}
																else if (part.key() == "name") {

																}
																else if (part.key() == "description") {

																}
																else if (part.key() == "required") {

																}
																else if (part.key() == "schema") {

																}
															}
														}
													}
												}
											}
											else if (option.key() == "responses") {
												if (option.value().is_object()) {
													auto codes = option.value().get<json>();

													for (auto& code : codes.items()) {
														// Process response code & data
													}
												}
											}
											else if (option.key() == "security") {
												if (option.value().is_array()) {
													auto security = option.value().get<json>();

													for (auto& sec : security) {

													}
												}
											}
										}
									}

									http_routes.push_back(route);
								}
							}
						}
					}

					if (values["securityDefinitions"].is_object()) {
						auto securities = values["securityDefinitions"].get<json>();

						for (auto& item : securities.items()) {
							if (item.value().is_object()) {
								SecurityDefinition definition;

								if (item.value()["name"].is_string())
									definition.name = item.value()["name"].get<std::string>();
								else
									definition.name = item.key();

								if (item.value()["type"].is_string())
									definition.type = item.value()["type"].get<std::string>();
								if (item.value()["authorizationUrl"].is_string())
									definition.url = item.value()["authorizationUrl"].get<std::string>();
								if (item.value()["flow"].is_string())
									definition.flow = item.value()["flow"].get<std::string>();
							}
						}
					}

					if (values["definitions"].is_object()) {
						auto definitions = values["definitions"].get<json>();

						for (auto& def : definitions.items()) {
							auto model = new Model();
							model->name = def.key();

							if (def.value()["properties"].is_object()) {
								auto props = def.value()["properties"].get<json>();

								QueryBuilder::Field field;

								for (auto& prop : props.items()) {
									field.name = prop.key();

									if (prop.value().is_object()) {
										auto attributes = prop.value().get<json>();

										std::string type;
										std::string format;

										if (attributes["type"].is_string())
											type = attributes["type"].get<std::string>();
										if (attributes["format"].is_string())
											format = attributes["format"].get<std::string>();

										if (type == "string" && format == "")
											field.type = QueryBuilder::Field::Type::STRING;
										if (type == "string" && format == "date-time")
											field.type = QueryBuilder::Field::Type::DATETIME;
										if (type == "integer" && (format == "int32" || format == "int64"))
											field.type = QueryBuilder::Field::Type::INTEGER;
										if (type == "float")
											field.type = QueryBuilder::Field::Type::FLOAT;

										if (attributes["nullable"].is_boolean()) {
											field.not_null = !attributes["nullable"].get<bool>();
										}
									}

									model->schema.push_back(field);
								}
							}

							ORM::registerModel(model);
						}
					}
				}
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
		}
	}

	printDebug();
}
