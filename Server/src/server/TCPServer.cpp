#include "TCPServer.h"
#include "../http/Parser.h"

uv_loop_t* TCPServer::loop = uv_default_loop();
int TCPServer::request_id = 1;

TCPServer::TCPServer(RequestCallback callback) :
	request_callback(callback),
	max_connections(128)
{
}

TCPServer::~TCPServer()
{
	delete loop;
	delete server;
}

void TCPServer::listen(const std::string& ip, uint16_t port)
{
	try {
		uv_timer_t gc_req;
		server = new uv_tcp_t();

		uv_tcp_init(loop, server);
		uv_ip4_addr(ip.c_str(), port, &addr);

		uv_tcp_bind(server, (const struct sockaddr*)&addr, 0);

		int status = uv_listen((uv_stream_t*)server, max_connections, &TCPServer::onConnect);

		if (status) {
			std::cout << "Listen error: \n" << uv_strerror(status) << std::endl;
		}

		std::cout << "Server listening on port " << std::to_string(port) << std::endl;

		uv_run(loop, UV_RUN_DEFAULT);
		uv_loop_close(loop);

		delete loop;
		delete server;
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

void TCPServer::onConnect(uv_stream_t* server, int status)
{
	if (status < 0) {
		std::cout << "onClientConnect error: " << uv_strerror(status) << std::endl;
		return;
	}

	client_t* client = new client_t();
	client->request_id = request_id;

	uv_tcp_init(loop, &client->handle);

	if (uv_accept(server, (uv_stream_t*)&client->handle) == 0) {
		std::cout << "Client connected" << std::endl;
		uv_read_start((uv_stream_t*)&client->handle, &TCPServer::onAllocate, &TCPServer::onRead);
	}
}

void TCPServer::onAllocate(uv_handle_t* handle, size_t size, uv_buf_t* buffer)
{
	buffer->base = new char[size];
	buffer->len = size;
}

void TCPServer::onRead(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
	if (nread >= 0) {
		try {
			auto request = Parser::parse(buf->base);

			std::cout << request->url << std::endl;
			std::cout << request->headers["Host"] << std::endl;
			std::cout << request->method << std::endl;
			std::cout << request->protocol << std::endl;

			uv_write_t* write_req = new uv_write_t();

			uv_buf_t buffer;
			auto response = "HTTP/1.1 200 OK\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 12\r\n\r\n"
				"hello world\n";

			buffer.base = (char*)response;
			buffer.len = strlen(response);

			uv_write(write_req, client, &buffer, 1, &TCPServer::onWrite);
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}
	else {
		if (nread != UV_EOF)
			printf("Read error %s\n", uv_err_name(nread));
	}

	delete buf->base;
	uv_close((uv_handle_t*)client, &TCPServer::onClose);
}

void TCPServer::onWrite(uv_write_t* req, int status)
{
	delete req;
}

void TCPServer::onClose(uv_handle_t* handle)
{
	client_t* client = (client_t*)handle->data;
	std::cout << "connection closed" << std::endl;
	delete client;
}
