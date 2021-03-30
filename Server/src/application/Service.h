#pragma once

class Webserver;

class Service {
public:
	Service(WebServer* server) {
		this->server = server;
	}

	virtual ~Service() {}
	virtual void init() {}

	WebServer* server;
};
