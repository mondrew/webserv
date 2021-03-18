#ifndef SERVERS_HPP
#define SERVERS_HPP

#include <iostream>
#include <vector>
#include "Config.hpp"

class Servers
{
	private:
		Config config;
		Servers();

	public:
		Servers(Config config);
		~Servers();
		Servers(Servers const & cp);
		Servers &operator=(Servers const & op);

		int		startServers();
		Config	getConfig();



};

#endif
