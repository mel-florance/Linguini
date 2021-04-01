#include "TCPListener.h"
#include "../orm/ORM.h"

TCPListener::TCPListener(const char* ip, int port) :
	ip(ip),
	port(port),
	handle(0),
	master({}),
	initialized(false),
	private_key_file(""),
	certificate_file(""),
	clients({}),
	fd_max(0)
{
	initialized = init();
}

int TCPListener::init()	
{

#ifdef PLATFORM_WINDOWS
	WSADATA data;
	WORD version = MAKEWORD(2, 2);

	auto status = WSAStartup(version, &data);

	if (status != 0) {
		ORM::logger.error("NETWORKING", "Can't init winsock: %d", WSAGetLastError());
		return 0;
	}
	else {
		ORM::logger.info("NETWORKING", "Initialized Winsock 2.2");
	}
#endif

	SSL_library_init();
	OpenSSL_add_all_algorithms();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();

	handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (handle == -1) {
		ORM::logger.error("NETWORKING", "Can't create socket");
		return 0;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	hint.sin_addr.s_addr = INADDR_ANY;
	int binding = bind(handle, (sockaddr*)&hint, sizeof hint);

	if (binding == -1) {
		ORM::logger.error("NETWORKING", "Can't bind socket");
	}
	
	ORM::logger.info("NETWORKING", "Initialized TCP socket.");

	return 1;
}

int TCPListener::run()
{
	if (initialized)
	{
		auto listening = listen(handle, 128);

		if (listening == -1) {
			ORM::logger.error("NETWORKING", "Can't listen socket");
			return 0;
		}

		FD_ZERO(&master);
		FD_SET(handle, &master);
		fd_max = handle;

		SSL_CTX* ctx = createSSLContext();
		configureSSLContext(ctx);

		bool running = true;
		ORM::logger.success("NETWORKING", "Web server listening on port %d", port);

		while (running)
		{
			fd_set copy = master;

			int socket_count = select(fd_max + 1, &copy, 0, 0, 0);

			if (socket_count == -1) {
				ORM::logger.error("NETWORKING", "Error selecting socket.");
				return;
			}

			for (int i = 0; i < fd_max; ++i)
			{
#ifdef PLATFORM_WINDOWS
				SOCKET sock = copy.fd_array[i];
#endif
#ifdef PLATFORM_LINUX
				SOCKET sock = i;
#endif
				sockaddr_in addr = { 0 };
				int addrlen = sizeof(addr);

				if (sock == handle)
				{
					uintptr_t client = accept(handle, (sockaddr*)&addr, &addrlen);
					FD_SET(client, &master);

					auto ssl = SSL_new(ctx);
					SSL_set_fd(ssl, client);
					auto status = SSL_accept(ssl);

					if (status <= 0) {
						ORM::logger.error("NETWORKING", "SSL Error: %s", ERR_error_string(ERR_get_error(), NULL));
					}
					else {
						TCPSocket* data = new TCPSocket();
						getpeername(sock, (sockaddr*)&addr, &addrlen);
						data->ip = inet_ntoa(addr.sin_addr);
						data->socket = client;
						data->ssl = ssl;
						data->websocket = false;

						clients[data->socket] = data;

						onClientConnected(data);
					} 
				}
				else
				{
					char buf[8192];
					memset(buf, 0, sizeof buf);

					auto client = clients.find(sock);

					if (client != clients.end())
					{
						int bytes_in = SSL_read(client->second->ssl, buf, sizeof(buf));

						//int bytes_in = recv(sock, buf, 4096, 0);

						TCPSocket* data = new TCPSocket();
						getpeername(sock, (sockaddr*)&addr, &addrlen);
						data->ip = inet_ntoa(addr.sin_addr);
						data->socket = sock;
						data->websocket = client->second->websocket;
						data->ssl = client->second->ssl;

						if (bytes_in <= 0)
						{
							onClientDisconnected(data);
							closesocket(sock);
							SSL_free(client->second->ssl);
							FD_CLR(sock, &master);

							clients.erase(client);
						}
						else {
							if (!data->websocket) {
								onMessageReceived(data, buf, bytes_in);
							}
							else {
								std::cout << "WEBSOCKET COM" << std::endl;

								char mask[4];
								char* payload = nullptr;
								uint16_t bits16 = *((uint16_t*)&buf[0]);
								uint8_t fin = (!!(bits16 & 0b0000000010000000));
								uint8_t rv1 = (!!(bits16 & 0b0000000001000000));
								uint8_t rv2 = (!!(bits16 & 0b0000000000100000));
								uint8_t rv3 = (!!(bits16 & 0b0000000000010000));
								uint8_t opc = ((bits16 & 0b0000000000001111));
								uint8_t msk = (!!(bits16 & 0b1000000000000000));
								uint64_t len = ((bits16 >> 8) & 0b0000000001111111);

								if (opc == Websocket_OpCode::CONNECTION_CLOSE_FRAME)
								{
									std::cout << "WEBSOCKET CLOSED " << opc << std::endl;
									clients[sock]->websocket = false;
								}
								else {
									if (len <= 125)
									{
										std::cout << "small data" << std::endl;
										mask[0] = buf[2];
										mask[1] = buf[3];
										mask[2] = buf[4];
										mask[3] = buf[5];
										payload = &buf[6];
									}
									else if (len == 126)
									{
										std::cout << "medium data" << std::endl;
										mask[0] = buf[4];
										mask[1] = buf[5];
										mask[2] = buf[6];
										mask[3] = buf[7];
										payload = &buf[8];
										uint32_t bits32 = *((uint32_t*)&buf[0]);
										len = ((bits32 & 0b00000000111111110000000000000000) >> 8)
											| ((bits32 & 0b11111111000000000000000000000000) >> 24);
									}
									else if (len == 127)
									{
										std::cout << "big data" << std::endl;
										mask[0] = buf[10];
										mask[1] = buf[11];
										mask[2] = buf[12];
										mask[3] = buf[13];
										payload = &buf[14];
										uint32_t bits64 = *((uint32_t*)&buf[4]);
										len = bits64 >> 5;
										len = len;
									}

									if (len < bytes_in)
									{
										std::cout << "Payload byte length: " << len << std::endl;
										for (size_t i = 0; i < len; ++i)
										{
											payload[i] = payload[i] ^ mask[i % 4];
										}

										std::cout << "client> " << payload << std::endl;
									}
									else
									{
										std::cout << "Data too big: " << len << std::endl;
									}

									json jason;
									jason["message"] = "salut le client";

									auto str = jason.dump();
									socketBroadcast(data, str.c_str(), str.size());
								}
							}
						}
					}
				}
			}
		}

		ORM::logger.warn("NETWORKING", "Web server stopped.");

		FD_CLR(handle, &master);
#ifdef PLATFORM_LINUX
		close();
#endif
#ifdef PLATFORM_WINDOWS
		WSACleanup();
		closesocket(handle);
#endif
	}

	return 0;
}

void TCPListener::sendToClient(TCPSocket* client, const char* message, int length)
{
	//send(client.socket, message, length, 0);
	SSL_write(client->ssl, message, length);
}

void TCPListener::broadcastToClients(int emitter, const char* message, int length)
{
	for (int i = 0; i < master.fd_count; ++i) {
		SOCKET out = master.fd_array[i];

		if (out != handle && out != emitter) {
			//sendToClient(out, message, length);
		}
	}
}

void TCPListener::socketEmit(TCPSocket* client, const char* message, int length)
{
	if (client->websocket) {
		char send_buffer[512];
		memset(send_buffer, 0, sizeof send_buffer);
		send_buffer[0] = 0b10000001;

		size_t txt_len = strlen(message);
		if (txt_len <= 125)
		{
			send_buffer[1] = txt_len;
			snprintf(&send_buffer[2], 510, message);
		}
		else
		{
			send_buffer[1] = 126;
			send_buffer[2] = ((txt_len >> 8) & 0xff);
			send_buffer[3] = ((txt_len >> 0) & 0xff);
			//*((uint32*)&send_buffer[2]) = txt_len;
			snprintf(&send_buffer[4], 508, message);
		}

		auto send_success = SSL_write(client->ssl, send_buffer, (int32_t)strlen(send_buffer));

		if (send_success == SOCKET_ERROR)
		{
			clients[client->socket]->websocket = false;
			std::cout << "Failed send" << std::endl;
		}
	}
}

void TCPListener::socketBroadcast(TCPSocket* emitter, const char* message, int length)
{
	if (emitter->websocket) {
		for (auto& client : clients) {
			if (client.second->socket != emitter->socket
			&& client.second->websocket) {
				socketEmit(client.second, message, length);
			}
		}
	}
}

SSL_CTX* TCPListener::createSSLContext()
{
	SSL_CTX* ctx = SSL_CTX_new(SSLv23_server_method());

	if (!ctx) {
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	return ctx;
}

void TCPListener::configureSSLContext(SSL_CTX* ctx)
{
	SSL_CTX_set_ecdh_auto(ctx, 1);

	if (SSL_CTX_use_certificate_file(ctx, certificate_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	if (SSL_CTX_use_PrivateKey_file(ctx, private_key_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	ORM::logger.info("NETWORKING", "Configured SSL certificates.");
}
