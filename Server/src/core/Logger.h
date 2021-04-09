#pragma once

#include <iostream>
#include <ctime>
#include <map>
#include <cstdarg>

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include "Utils.h"

#ifdef PLATFORM_WINDOWS
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
#endif
#ifdef PLATFORM_LINUX
	enum Color {
		BLACK = 30,
		BLUE = 34,
		GREEN = 32,
		CYAN = 36,
		RED = 31,
		MAGENTA = 95,
		BROWN = 94,
		LIGHTGRAY = 7,
		DARKGRAY = 8,
		LIGHTBLUE = 45,
		LIGHTGREEN = 46,
		LIGHTCYAN = 51,
		LIGHTRED = 196,
		LIGHTMAGENTA = 201,
		YELLOW = 11,
		WHITE = 15
	};
#endif

struct ColorModifier {
	ColorModifier(Color code) : code(code) {}
	Color code;

	friend std::ostream& operator << (std::ostream& stream, const ColorModifier& color) {
		return stream << "\033[" << color.code << "m";
	}
};

class Logger {
public:

	Logger() {}

	static void log(const std::string& category, const char* format, ...) {
		char buffer[4096];
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, 4095, format, args);

		std::string prefix;

#ifdef PLATFORM_WINDOWS
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, WHITE);
		prefix = buffer;
#endif
#ifdef PLATFORM_LINUX
	ColorModifier color(Color::RED);
	std::stringstream str;
	str << color;
	prefix += str.str();
	prefix += buffer;
#endif
		prefix += buffer;
		std::string line = "[" + getTime() + "][" + category + "] " + prefix;
		logs[category].push_back(line);
		std::cout << line << '\n';
	}

	static void info(const std::string& category, const char* format, ...) {
		char buffer[4096];
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, 4095, format, args);

#ifdef PLATFORM_WINDOWS
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, CYAN);
#endif

		std::string line = "[" + getTime() + "][" + category + "] " + buffer;
		logs[category].push_back(line);
		std::cout << line << '\n';

#ifdef PLATFORM_WINDOWS
		SetConsoleTextAttribute(hConsole, WHITE);
#endif
	}

	static void warn(const std::string& category, const char* format, ...) {
		char buffer[4096];

		va_list args;
		va_start(args, format);
		vsnprintf(buffer, 4095, format, args);

#ifdef PLATFORM_WINDOWS
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, YELLOW);
#endif

		std::string line = "[" + getTime() + "][" + category + "] " + buffer;
		logs[category].push_back(line);
		std::cout << line << '\n';

#ifdef PLATFORM_WINDOWS
		SetConsoleTextAttribute(hConsole, WHITE);
#endif
	}

	static void success(const std::string& category, const char* format, ...) {
		char buffer[4096];

		va_list args;
		va_start(args, format);
		vsnprintf(buffer, 4095, format, args);

#ifdef PLATFORM_WINDOWS
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, GREEN);
#endif

		std::string line = "[" + getTime() + "][" + category + "] " + buffer;
		logs[category].push_back(line);
		std::cout << line << '\n';

#ifdef PLATFORM_WINDOWS
		SetConsoleTextAttribute(hConsole, WHITE);
#endif
	}

	static void error(const std::string& category, const char* format, ...) {
		char buffer[4096];

		va_list args;
		va_start(args, format);
		vsnprintf(buffer, 4095, format, args);

#ifdef PLATFORM_WINDOWS
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, RED);
#endif

		std::string line = "[" + getTime() + "][" + category + "] " + buffer;
		logs[category].push_back(line);
		std::cout << line << '\n';

#ifdef PLATFORM_WINDOWS
		SetConsoleTextAttribute(hConsole, WHITE);
#endif
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

