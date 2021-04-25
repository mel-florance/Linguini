#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>
#include <uv.h>

typedef struct {
	uv_write_t req;
	uv_buf_t buf;
} write_req_t;

typedef struct {
	uv_tcp_t handle;
	uv_write_t write_req;
	int request_id;
} client_t;

struct client_request_data {
	time_t start;
	char* text;
	size_t text_len;
	char* response;
	int work_started;
	uv_tcp_t* client;
	uv_work_t* work_req;
	uv_write_t* write_req;
	uv_timer_t* timer;
};

namespace HTTP {
	class Buffer : public std::stringbuf {
	public:
		Buffer(std::ostream& stream) {}
	};

	class Request {
		std::string url;
		std::string method;
		std::string body;
		int status;
		std::unordered_map<const std::string, const std::string> headers;
	};

	class Response : std::ostream
	{
	public:
		Response() :
			std::ostream(&buffer),
			buffer(stream) {
		}
	private:
		Buffer buffer;
		std::stringstream stream;
	};
}

class TCPServer
{
public:
	typedef std::function<void(HTTP::Request&, HTTP::Response&)> RequestCallback;

	TCPServer(RequestCallback callback);
	~TCPServer();

	void listen(const std::string& ip, uint16_t port);

private:
	static void onConnect(uv_stream_t* server, int status);
	static void onAllocate(uv_handle_t* handle, size_t size, uv_buf_t* buffer);
	static void onRead(uv_stream_t* client, ssize_t read, const uv_buf_t* buf);
	static void onWrite(uv_write_t* req, int status);
	static void onClose(uv_handle_t* handle);

	std::string ip;
	sockaddr_in addr;
	static uv_loop_t* loop;
	uv_tcp_t* server;
	RequestCallback request_callback;
	static int request_id;

	int max_connections;
};

