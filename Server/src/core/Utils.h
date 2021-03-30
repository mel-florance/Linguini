#pragma once

#include <vector>
#include <string>
#include <locale>  
#include <sstream>  
#include <cctype>
#include <iomanip>
#include <random>

static const std::string base64_chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

class Utils 
{
public:
	static inline std::vector<std::string> split(
		const std::string& str, 
		const std::string& delimiter
	) {
		std::vector<std::string> strings;
		auto pos = 0, prev = 0;

		while ((pos = str.find(delimiter, prev)) != std::string::npos) {
			strings.push_back(str.substr(prev, pos - prev));
			prev = pos + 1;
		}

		strings.push_back(str.substr(prev));

		return strings;
	}

	static inline std::string join(
		const std::vector<std::string>& str, 
		const std::string& delimiter = ","
	) {
		std::ostringstream stream;

		for (auto& i : str) {
			if (&i != &str[0])
				stream << delimiter;

			stream << i;
		}

		return stream.str();
	}

	static inline std::string pad(int value, int n = 1) {
		auto str = std::to_string(value);
		return value <= 9 ? std::string(n, '0') + str : str;
	};

	static inline std::string upper(std::string str) {
		for (auto i = 0; i < str.length(); ++i)
			str[i] = toupper(str[i]);

		return str;
	}

	static inline std::string escape(const std::string& str, const std::string& characters = "\"\r\n\t") {
		std::vector<std::string> chars;

		for (auto c : str) {
			chars.push_back(strchr(characters.c_str(), c)
				? "\\" + std::string(1, c)
				: std::string(1, c));
		}

		return join(chars, "");
	}

	static inline std::string lower(std::string str) {
		for (auto i = 0; i < str.size(); i++)
			str[i] = std::tolower(str[i], std::locale());

		return str;
	}

	static inline std::string strip(const std::string& str, char character = '"')
	{
		std::string out(str);
		out.erase(std::remove(out.begin(), out.end(), character), out.end());
		return out;
	}

	static inline std::string& ltrim(std::string& str) {
		size_t start = str.find_first_not_of(" \t\r\n\v\f");

		if (std::string::npos != start)
			str = str.substr(start);

		return str;
	}

	static inline std::string& trim(std::string& str) {
		return ltrim(rtrim(str));
	}

	static inline std::string& rtrim(std::string& str) {
		size_t end = str.find_last_not_of(" \t\r\n\v\f");

		if (std::string::npos != end)
			str = str.substr(0, end + 1);

		return str;
	}

	static inline std::string bytesToSize(uint64_t size)
	{
		const char* suffix[5] = { "B", "KB", "MB", "GB", "TB" };
		char length = sizeof(suffix) / sizeof(suffix[0]);

		int i = 0;
		float bytes = float(size);

		if (size > 1024) {
			for (i = 0; (size / 1024) > 0 && i < length - 1; i++, size /= 1024)
				bytes = size / 1024.0f;
		}

		static char output[200];
		sprintf(output, "%.02lf %s", bytes, suffix[i]);

		return std::string(output);
	}

	static inline std::string url_encode(const std::string& value) {
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << std::hex;

		for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
			std::string::value_type c = (*i);

			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
				escaped << c;
				continue;
			}

			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << int((unsigned char)c);
			escaped << std::nouppercase;
		}

		return escaped.str();
	}

	static inline std::string url_decode(std::string str) {
		std::string out;
		char character;
		int i, buffer, length = str.length();

		for (i = 0; i < length; i++) {
			if (str[i] != '%') {
				if (str[i] == '+')
					out += ' ';
				else
					out += str[i];
			}
			else {
				sscanf(str.substr(i + 1, 2).c_str(), "%x", &buffer);
				character = static_cast<char>(buffer);
				out += character;
				i += 2;
			}
		}

		return out;
	}

	static inline bool is_base64(unsigned char c) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	}

	inline static std::string base64_encode(const std::string& str) {
		int i = 0, j = 0;
		std::string out;
		unsigned char src[3];
		unsigned char chars[4];
		auto size = str.size();
		auto data = str.c_str();

		while (size--)
		{
			src[i++] = *(data++);

			if (i == 3) {
				chars[0] = (src[0] & 0xfc) >> 2;
				chars[1] = ((src[0] & 0x03) << 4) + ((src[1] & 0xf0) >> 4);
				chars[2] = ((src[1] & 0x0f) << 2) + ((src[2] & 0xc0) >> 6);
				chars[3] = src[2] & 0x3f;

				for (i = 0; (i < 4); i++)
					out += base64_chars[chars[i]];

				i = 0;
			}
		}

		if (i)
		{
			for (j = i; j < 3; j++)
				src[j] = '\0';

			chars[0] = (src[0] & 0xfc) >> 2;
			chars[1] = ((src[0] & 0x03) << 4) + ((src[1] & 0xf0) >> 4);
			chars[2] = ((src[1] & 0x0f) << 2) + ((src[2] & 0xc0) >> 6);
			chars[3] = src[2] & 0x3f;

			for (j = 0; (j < i + 1); j++)
				out += base64_chars[chars[j]];

			while ((i++ < 3))
				out += '=';
		}

		return out;
	}

	inline static std::string base64_decode(std::string const& str)
	{
		int length = str.size(), i = 0, j = 0, in = 0;
		unsigned char src[4], chars[3];
		std::string ret;

		while (length-- && (str[in] != '=') && is_base64(str[in])) {
			src[i++] = str[in]; in++;

			if (i == 4) {
				for (i = 0; i < 4; i++)
					src[i] = base64_chars.find(src[i]);

				chars[0] = (src[0] << 2) + ((src[1] & 0x30) >> 4);
				chars[1] = ((src[1] & 0xf) << 4) + ((src[2] & 0x3c) >> 2);
				chars[2] = ((src[2] & 0x3) << 6) + src[3];

				for (i = 0; (i < 3); i++)
					ret += chars[i];

				i = 0;
			}
		}

		if (i) {
			for (j = i; j < 4; j++)
				src[j] = 0;

			for (j = 0; j < 4; j++)
				src[j] = base64_chars.find(src[j]);

			chars[0] = (src[0] << 2) + ((src[1] & 0x30) >> 4);
			chars[1] = ((src[1] & 0xf) << 4) + ((src[2] & 0x3c) >> 2);
			chars[2] = ((src[2] & 0x3) << 6) + src[3];

			for (j = 0; (j < i - 1); j++)
				ret += chars[j];
		}

		return ret;
	}

	inline static std::string random_string(std::string::size_type length)
	{
		static auto& chrs = "0123456789"
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		thread_local static std::mt19937 rg{ std::random_device{}() };
		thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

		std::string s;

		s.reserve(length);

		while (length--)
			s += chrs[pick(rg)];

		return s;
	}

	inline static const char* getDateRFC822() {
		time_t current;
		char date[40];
		time(&current);

		strftime(date, sizeof date, "%a, %d %b %Y %T %z", localtime(&current));

		return date;
	}

	static std::string getMimeType(const std::string& extension);
};
