#pragma once

#include "../http/Request.h"
#include "../http/Response.h"

class Middleware {
public:
	Middleware() {}
	virtual ~Middleware() {}

	virtual bool use(Request& request, Response& response) { return false; }
};

