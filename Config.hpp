#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include "Server.hpp"

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
};

#endif
