#pragma once

#include <map>
#include <any>
#include <string>
#include <vector>
#include <variant>
#include <utility>
#include <unordered_map>

template<typename T>
struct Identity {
	typedef T type;
};

struct Time {
	unsigned year;
	unsigned month;
	unsigned day;
	unsigned hour;
	unsigned minute;
	unsigned second;
};

typedef std::vector<std::any> Array;
typedef std::unordered_map<std::string, std::any> Object;
