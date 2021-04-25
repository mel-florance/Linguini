//#include <iostream>

//#include "http/WebServer.h"
#include "server/TCPServer.h"
//
//#include "services/Mailer.h"
//#include "middlewares/BasicAuth.h"
//#include "middlewares/JsonWebToken.h"
//
//#include "application/StatusController.h"

int main()
{
	auto server = TCPServer([=](auto& req, auto& res) {

	});

	server.listen("0.0.0.0", 80);

	/*WebServer server(443);

	server.setPrivateKeyFile("./project/config/ssl/privkey.pem");
	server.setCertificateFile("./project/config/ssl/cert.pem");
	server.addStatic("/assets", "./project/static");

	server.registerService<Mailer>("Mailer");

	server.registerController<StatusController>("Status");

	server.registerMiddleware<BasicAuth>("BasicAuth");
	server.registerMiddleware<JsonWebToken>("jwt");

	server.run();*/

	return 0;
}
