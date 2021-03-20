#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <vector>
#include "ServerConf.hpp"
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
		std::vector<ServerConf>	servers;

	public:
		Config();
		~Config();
		Config(Config const &cp);
		Config & operator=(Config const &op);

		std::vector<ServerConf>	getServers();
		void					addServer(ServerConf server);
		void					createConfig();

};

#endif
