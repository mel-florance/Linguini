#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <memory>

struct Request {
	Request() = default;

	int status;
	std::string url;
	std::string method;
	std::string protocol;
	std::unordered_map<std::string, std::string> headers;
	std::unordered_map<std::string, std::string> params;
	std::unordered_map<std::string, std::string> body;
	std::unordered_map<std::string, std::string> query;
	std::optional<std::pair<std::string, std::string>> credentials;
};

class Parser
{
public:
	static std::shared_ptr<Request> parse(const char* str);
};


