#pragma once

#include <iostream>
#include <ctime>
#include <map>
#include <Windows.h>

#include "Utils.h"

class Logger {
public:

	Logger() {}

	enum Color {
		BLACK,
		BLUE,
		GREEN,
		CYAN,
		RED,
		MAGENTA,
		BROWN,
		LIGHTGRAY,
		DARKGRAY,
		LIGHTBLUE,
		LIGHTGREEN,
		LIGHTCYAN,
		LIGHTRED,
		LIGHTMAGENTA,
		YELLOW,
		WHITE
	};

	static void log(const std::string& category, const char* format, ...) {
		char buffer[4096];

    va_list args;
		va_start(args, format);
		vsnprintf(buffer, 4095, format, args);

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hConsole, WHITE);
		std::string line = "[" + getTime() + "][" + category + "] " + buffer;
		logs[category].push_back(line);
		std::cout << line << '\n';
	}

	static void info(const std::string& category, const char* format, ...) {
		char buffer[4096];

		va_list args;
		va_start(args, format);
		vsnprintf(buffer, 4095, format, args);

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hConsole, CYAN);
		std::string line = "[" + getTime() + "][" + category + "] " + buffer;
		logs[category].push_back(line);
		std::cout << line << '\n';
		SetConsoleTextAttribute(hConsole, WHITE);
	}

	static void warn(const std::string& category, const char* format, ...) {
		char buffer[4096];

		va_list args;
		va_start(args, format);
		vsnprintf(buffer, 4095, format, args);

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hConsole, YELLOW);
		std::string line = "[" + getTime() + "][" + category + "] " + buffer;
		logs[category].push_back(line);
		std::cout << line << '\n';
		SetConsoleTextAttribute(hConsole, WHITE);
	}

	static void success(const std::string& category, const char* format, ...) {
		char buffer[4096];

		va_list args;
		va_start(args, format);
		vsnprintf(buffer, 4095, format, args);

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hConsole, GREEN);
		std::string line = "[" + getTime() + "][" + category + "] " + buffer;
		logs[category].push_back(line);
		std::cout << line << '\n';
		SetConsoleTextAttribute(hConsole, WHITE);
	}

	static void error(const std::string& category, const char* format, ...) {
		char buffer[4096];

		va_list args;
		va_start(args, format);
		vsnprintf(buffer, 4095, format, args);

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hConsole, RED);
		std::string line = "[" + getTime() + "][" + category + "] " + buffer;
		logs[category].push_back(line);
		std::cout << line << '\n';
		SetConsoleTextAttribute(hConsole, WHITE);
	}

	static std::string getTime() {
		std::time_t t = std::time(0);
		std::tm* now = std::localtime(&t);

		return std::to_string(now->tm_year + 1900) + '-'
			+ Utils::pad(now->tm_mon + 1) + '-'
			+ Utils::pad(now->tm_mday) + ' '
			+ Utils::pad(now->tm_hour) + ':'
			+ Utils::pad(now->tm_min) + ':'
			+ Utils::pad(now->tm_sec);
	}

	static std::map<std::string, std::vector<std::string>> logs;
};

