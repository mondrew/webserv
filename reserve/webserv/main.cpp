#include "Config.hpp"
#include "Servers.hpp"

void printConfig(Config config)
{
	//Get servers
	std::vector<ServerConf> servers = config.getServers();

	//Get specific server
	std::vector<ServerConf>::iterator it_beg = servers.begin();
	std::vector<ServerConf>::iterator it_end = servers.end();
	std::cout << "\n\n";
	while (it_beg != it_end)
	{
		std::cout << "server {\n";
		std::cout << "	server_name			" << (*it_beg).getServer_name() << ";\n";
		std::cout << "	listen				" << (*it_beg).getPortListen() << ";\n";
		std::cout << "	error page 402		" << (*it_beg).getDefault_err_page_402() << ";\n";
		std::cout << "	error page 404		" << (*it_beg).getDefault_err_page_404() << ";\n";

		// Get all locations
		std::vector<Location> locations = (*it_beg).getLocations();

		std::vector<Location>::iterator it_beg_loc = locations.begin();
		std::vector<Location>::iterator it_end_loc = locations.end();
		// Get specific locations
		while (it_beg_loc != it_end_loc)
		{
			std::cout << "	location \\" << (*it_beg_loc).getLocationPath() << "{\n";
			std::cout << "		index " << (*it_beg_loc).getIndex() << "\n";
			std::cout << "		root " << (*it_beg_loc).getRoot() << "\n";
			std::cout << "		autoindex ";
			if ((*it_beg_loc).isAutoindex()) std::cout << "on;\n";
			else std::cout << "off;\n";
			std::cout << "	}\n";
			it_beg_loc++;
		}
		std::cout << "}\n";
		it_beg++;
	}
}

int main()
{
	Config config;
	//Autogenerate config. See "Config -> createConfig()"
	config.createConfig();
	printConfig(config);

	Servers servers = Servers(config);
	servers.startServers();
	return (0);
}
