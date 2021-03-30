#pragma once

#include "../application/Middleware.h"

class JsonWebToken : public Middleware {
public:
	JsonWebToken() : Middleware() {};

	bool use(Request& request, Response& response) override {
		std::cout << "hey im  jwt" << std::endl;
		return true;
	}
};
