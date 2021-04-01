#pragma once

#ifdef PLATFORM_WINDOWS
	#pragma comment (lib, "ws2_32.lib")
	#pragma comment (lib, "fwpuclnt.lib")

	#include <WS2tcpip.h>
	#include <winsock2.h>
#endif
#ifdef PLATFORM_LINUX
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <netdb.h>

	typedef int SOCKET;
	#define SOCKET_ERROR -1
#endif

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <string>
#include <vector>
#include <future>
#include <regex>
#include <iterator>
#include <unordered_map>
#include <time.h>

#include "../core/ThreadPool.h"
#include "../application/Service.h"
#include "../core/Utils.h"
#include "../orm/ORM.h"
#include "../core/Inja.h"

class WebServer;

class Mailer : public Service {
public:
	Mailer(WebServer* server) :
		Service(server),
		thread_pool(new ThreadPool(4))
	{
	}

	virtual ~Mailer() {
	}

	enum Command {
		CONNECT,
		AUTH_DATA,
		HELO,
		AUTH_LOGIN,
		MAIL_FROM,
		RCPT_TO,
		DATA,
		BODY,
		HEADER,
		EOT,
		QUIT
	};

	enum ReplyCode {
		UNABLE_TO_CONNECT                 = 101,
		CONNECTION_REFUSED                = 111,
		SYSTEM_STATUS_MESSAGE             = 214,
		SERVER_READY                      = 220,
		SERVER_CLOSING                    = 221,
		REQUEST_ACTION_COMPLETED          = 250,
		USER_NOT_LOCAL_WILL_FORWARD       = 251,
		CANOT_VERIFY_USER                 = 252,
		BEGIN_BODY_MESSAGE                = 354,
		TIMEOUT_CONNECTION                = 420,
		SERVICE_UNAVAILABLE               = 421,
		RECIPIENT_EXCEEDED_STORAGE        = 422,
		OUT_OF_MEMORY                     = 431,
		QUEUE_STOPPED                     = 432,
		SERVER_NOT_RESPONDING             = 441,
		CONNECTION_DROPPED                = 442,
		HOP_COUNT_EXCEEDED                = 446,
		MESSAGE_TIMED_OUT                 = 447,
		ROUTING_ERROR                     = 449,
		USER_MAILBOX_UNAVAILABLE          = 450,
		REQUEST_ACTION_ABORTED            = 451,
		SERVER_STORAGE_LIMIT_EXCEEDED     = 452,
		ANTI_SPAM_FILTER_ERROR            = 471,
		COMMAND_SYNTAX_ERROR              = 500,
		ARGUMENTS_SYNTAX_ERROR            = 501,
		COMMAND_NOT_IMPLEMENTED           = 502,
		BAD_SEQUENCE                      = 503,
		COMMAND_PARAMETER_NOT_IMPLEMENTED = 504,
		BAD_EMAIL_ADDRESS                 = 510,
		DNS_HOST_ERROR                    = 512,
		ADDRESS_TYPE_INCORRECT            = 513,
		MESSAGE_LIMIT_EXCEEDED            = 523,
		AUTHENTICATION_ERROR              = 530,
		MESSAGE_REJECTED                  = 541,
		EMAIL_INEXISTANT                  = 550,
		NON_LOCAL_USER_INVALID_ADDRESS    = 551,
		EXCEEDED_STORAGE_ALLOCATION       = 552,
		INVALID_MAILBOX_NAME              = 553,
		TRANSACTION_FAILED                = 554
	};

	inline std::string cmdToStr(Command cmd) {
		switch (cmd) {
		case Command::CONNECT: return "";
		case Command::AUTH_DATA: return "";
		case Command::HELO: return "HELO";
		case Command::AUTH_LOGIN: return "AUTH LOGIN";
		case Command::MAIL_FROM: return "MAIL FROM";
		case Command::RCPT_TO: return "RCPT TO";
		case Command::DATA: return "DATA";
		case Command::BODY: return "\n";
		case Command::HEADER: return "";
		case Command::EOT: return ".\r\n";
		case Command::QUIT: return "QUIT";
		}
	}

	struct Message {
		const char* from;
		const char* to;
		const char* subject;
		const char* body = "";
		const char* cc = "";
		const char* bcc = "";
		unsigned int priority = 3;

		inline void setHeader(
			const std::string& name,
			const std::string& value
		) {
			headers[name] = value;
		}

		std::unordered_map<std::string, std::string> headers;
	};

private:
	inline const char* sendCommand(
		SOCKET handle,
		Command cmd,
		const std::string& data = "",
		bool separator = false,
		bool brace = false,
		bool isHeader = false
	) {
		std::string str = "";
		char out[8192];
		memset(out, 0, sizeof out);

		auto command = cmdToStr(cmd);

		str += command;
		str += separator ? ": " : (command.size() > 0 && command != "\n" ? " ": "");
		str += brace ? "<" : "";

		str += data;

		str += brace ? ">" : "";
		str += isHeader ? "\n" : "\r\n";

		auto in = str.c_str();

		if (cmd != CONNECT) {
			send(handle, in, str.size(), 0);
			ORM::logger.info("MAILER", "TX: %s", Utils::strip(in, '\n').c_str());
		}
		else {
			ORM::logger.info("MAILER", "TX: CONNECT");
		}
	
		if (!isHeader) {
			recv(handle, out, sizeof out, 0);

			auto ack = verifyAck(out);
			auto str = Utils::escape(Utils::split(out, "\r\n").at(0)).c_str();

			if (strlen(str) > 0) {
				if (ack == -1 || ack >= 400)
					ORM::logger.error("MAILER", "RX: %s", str);
				else
					ORM::logger.success("MAILER", "RX: %s", str);
			}
		}

		return out;
	}

	inline void sendHeader(
		SOCKET handle,
		const std::string& key,
		const std::string& value,
		bool brace = false
	) {
		auto data = key + ": ";
		data += brace ? "<" : "";
		data += value;
		data += brace ? ">" : "";

		sendCommand(handle, HEADER, data.c_str(), false, false, true);
	}

	inline int verifyAck(const char* response)
	{
		auto lines = Utils::split(response, "\r\n");

		if (lines.size() > 0) {
			auto parts = Utils::split(lines.at(0), " ");
			auto code = std::atoi(parts.at(0).c_str());

			return code;
		}

		return -1;
	}

	inline SOCKET initSocket(const std::string& hostname)
	{
		auto host = gethostbyname(hostname.c_str());
		auto port = ORM::env.get("SMTP_PORT");

		if (!host) {
			ORM::logger.error("MAILER", "Cannot find hostname: %s", hostname.c_str());
			return 1;
		}

		auto handle = socket(PF_INET, SOCK_STREAM, 0);

		if (handle == -1) {
			ORM::logger.error("MAILER", "Cannot create mailer socket.");
			return 1;
		}

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(std::atoi(port.c_str()));
		addr.sin_addr = *((in_addr*)*host->h_addr_list);

		if (connect(handle, (sockaddr*)&addr, sizeof(addr))) {
			ORM::logger.error("MAILER", "Cannot connect mailer socket.");
			return 1;
		}

		return handle;
	}

public:
	inline void Send(
		const std::string& view,
		const Message& message,
		const json& data
	) {
		auto f1 = thread_pool->addTask([=]
		{
			auto hostname = ORM::env.get("SMTP_HOST");
			auto username = ORM::env.get("SMTP_USERNAME");
			auto password = ORM::env.get("SMTP_PASSWORD");

			auto handle = initSocket(hostname);

			if (handle) {
				sendCommand(handle, CONNECT);
				sendCommand(handle, HELO, hostname.c_str());

				sendCommand(handle, AUTH_LOGIN);
				sendCommand(handle, AUTH_DATA, Utils::base64_encode(username));
				sendCommand(handle, AUTH_DATA, Utils::base64_encode(password));

				sendCommand(handle, MAIL_FROM, message.from, true, true);
				sendCommand(handle, RCPT_TO, message.to, true, true);
				sendCommand(handle, DATA);

				sendHeader(handle, "Message-Id", Utils::random_string(32) + "@mx.virax.dev", true);
				sendHeader(handle, "Date", Utils::getDateRFC822());
				sendHeader(handle, "X-Mailer", "OhMyMailer");

				sendHeader(handle, "X-Priority", std::to_string(message.priority));
				sendHeader(handle, "Return-Path", message.from);
				sendHeader(handle, "Reply-To", message.from);
				sendHeader(handle, "Cc", message.cc);
				sendHeader(handle, "Bcc", message.bcc);
				sendHeader(handle, "Subject", message.subject);
				sendHeader(handle, "From", message.from);
				sendHeader(handle, "To", message.to);

				auto boundary = Utils::random_string(13) + "-alt";
				sendHeader(handle, "MIME-Version", "1.0");
				sendHeader(handle, "Content-Type", "multipart/alternative; boundary=\"boundary-type-" + boundary + "\"");

				Environment env;
				std::string html;
				std::string output;

				try {
					Template tpl = env.parse_template("./project/views/" + view + ".html");
					html = env.render(tpl, data);
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}

				// TODO: cleanup this part
				std::string styles;
				std::string plain_text;
				std::regex_replace(std::back_inserter(styles), html.begin(), html.end(), std::regex("<style([\\s\\S]+?)</style>"), "$2");
				std::regex_replace(std::back_inserter(plain_text), styles.begin(), styles.end(), std::regex("<[^>]*>"), "$2");
				
				output += "--boundary-type-" + boundary + "\r\n";
				output += "Content-type: text/plain;charset=\"iso-8859-1\"\r\n";
				output += "Content-Transfer-Encoding: 8bit\r\n\r\n";
				output += Utils::strip(Utils::strip(plain_text, '\t'), '\r\n');

				output += "\r\n\r\n--boundary-type-" + boundary + "\r\n";
				output += "Content-type: text/html;charset=\"iso-8859-1\"\r\n";
				output += "Content-Transfer-Encoding: 8bit\r\n\r\n";
				output += Utils::strip(Utils::strip(html, '\n'), '\t');

				output += "\r\n\r\n--boundary-type-" + boundary;

				sendCommand(handle, BODY, html.size() > 0 ? output : message.body, false, false, true);

				sendCommand(handle, EOT, "", false, false, true);
				sendCommand(handle, QUIT);

#ifdef PLATFORM_WINDOWS
				closesocket(handle);
#endif
#ifdef PLATFORM_LINUX
				close(handle);
#endif
			}

			return true;
		});
	}

	ThreadPool* thread_pool;
};
