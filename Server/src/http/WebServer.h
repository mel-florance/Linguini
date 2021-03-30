#pragma once

#include <string>
#include <fstream>
#include <unordered_map>
#include <functional>
#include <memory>
#include <filesystem>

#include "Status.h"
#include "Router.h"
#include "TCPListener.h"

#include "../application/Controller.h"
#include "../application/Middleware.h"
#include "../application/Service.h"
#include "../core/Logger.h"
#include "../core/EnvParser.h"
#include "UAParser.h"

class WebServer : public TCPListener
{
public:
	WebServer(int port, const char* ip = "0.0.0.0");

	template<typename T>
	inline void registerController(const std::string& name) {
		controllers[name] = std::make_unique<T>(this);
		logger.info("SERVER", "Registered controller %s", name);
	};

	template<typename T>
	inline void registerMiddleware(const std::string& name) {
		middlewares[name] = std::make_unique<T>();
		logger.info("SERVER", "Registered middleware %s", name);
	};

	template<typename T>
	inline void registerService(const std::string& name) {
		services[name] = std::make_shared<T>(this);
		logger.info("SERVER", "Registered service %s", name);
	};


	template<typename T>
	inline std::shared_ptr<T> getService(const std::string& name) {
		auto service = services.find(name);

		if (service != services.end()) {
			return std::dynamic_pointer_cast<T>(service->second);
		}

		return nullptr;
	}

	inline void addStatic(const std::string& alias, const std::string& directory) {
		mount_points[alias] = directory;
		logger.info("SERVER", "Registered mount point %s -> %s", alias, directory);
	}

	inline void removeStatic(const std::string& alias) {
		auto it = mount_points.find(alias);

		if (it != mount_points.end())
			mount_points.erase(it);
	}

	inline void setPrivateKeyFile(const std::string& path) {
		private_key_file = path;
	}

	inline void setCertificateFile(const std::string& path) {
		certificate_file = path;
	}


	static std::unordered_map<std::string, std::string> mount_points;
	static std::unordered_map<std::string, std::unique_ptr<Controller>> controllers;
	static std::unordered_map<std::string, std::unique_ptr<Middleware>> middlewares;
	static std::unordered_map<std::string, std::shared_ptr<Service>> services;

	Router router;
protected:
	void onClientConnected(TCPSocket* client) override;
	void onClientDisconnected(TCPSocket* client) override;
	void onMessageReceived(TCPSocket* client, const char* message, int length) override;

	UAParser uaparser;
	Logger logger;
	ORM orm;
};
