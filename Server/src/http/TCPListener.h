#pragma once

#ifdef PLATFORM_WINDOWS
	#pragma comment (lib, "ws2_32.lib")
	#pragma comment(lib, "fwpuclnt.lib")

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
#endif

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

#include <stdio.h>
#include <string>
#include <unordered_map>

class TCPListener
{
public:
	TCPListener(const char* ip, int port);

	int init();
	int run();

	enum Websocket_OpCode : uint8_t {
		CONTINUATION_FRAME = 0,
		TEXT_FRAME = 1,
		BINARY_FRAME = 2,
		CONNECTION_CLOSE_FRAME = 8,
		PING_FRAME = 9,
		PONG_FRAME = 10
	};

	enum SubProtocol : uint8_t {
		JSON
	};

	struct TCPSocket {
		TCPSocket() :
			ip(""),
			socket(0),
			ssl(nullptr),
			websocket(false),
			subprotocol(SubProtocol::JSON)
		{}

		std::string ip;
		unsigned int socket;
		SSL* ssl;
		bool websocket = false;
		SubProtocol subprotocol;
	};

	SSL_CTX* createSSLContext();
	void configureSSLContext(SSL_CTX* ctx);

	inline std::string subProtocolToStr(SubProtocol protocol) {
		switch (protocol) {
		case SubProtocol::JSON: return "json";
		}
	}

	inline SubProtocol strToSubProtocol(const std::string& protocol) {
		if (protocol == "json") return SubProtocol::JSON;
	}

	std::string private_key_file;
	std::string certificate_file;
	std::unordered_map<int, TCPSocket*> clients;

protected:
	virtual void onClientConnected(TCPSocket* client) {}
	virtual void onClientDisconnected(TCPSocket* client) {}
	virtual void onMessageReceived(TCPSocket* client, const char* message, int length) {}

	void sendToClient(TCPSocket* client, const char* message, int length);
	void broadcastToClients(int emitter, const char* message, int length);
	void socketEmit(TCPSocket* client, const char* message, int length);
	void socketBroadcast(TCPSocket* emitter, const char* message, int length);
private:
	bool initialized;
	const char* ip;
	int port;
	int handle;
	fd_set master;
};


