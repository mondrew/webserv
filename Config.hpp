#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <vector>
#include "Server.hpp"
#include <sys/socket.h>
#include "Logger.hpp"
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define MAX_USERS 1000

class Config
{
	private:
		std::vector<Server> servers;
	public:
		Config();
		virtual ~Config();
		Config(Config const &cp);
		Config & operator=(Config const &op);

		std::vector<Server> getServers();
		void addServer(Server server);
		void createConfig();
		int startServers();
};

#endif
