#ifndef SERVER_CONF_HPP
#define SERVER_CONF_HPP

#include <iostream>
#include <list>
#include <vector>
#include "Location.hpp"

class ServerConf
{
	private:
		std::vector<std::string>	server_names;
		std::string					host; // MONDREW: default value is 'localhost' (if there is no
										// host information in webserv.conf
		std::string					portListen;
		std::string					default_err_page_402;
		std::string					default_err_page_404;
		std::vector<Location>		locations;

	public:
		ServerConf();
		ServerConf(ServerConf const &cp);
		~ServerConf();
		ServerConf &operator=(ServerConf const &cp);

		std::vector<std::string>	getServer_names();
		void						setServer_names(std::vector<std::string> server_name);
		std::string					getHost();
		void						setHost(std::string a_host);
		std::string					getPortListen();
		void						setPortListen(std::string portListen);
		std::string					getDefault_err_page_402();
		void						setDefault_err_page_402(std::string default_err_page_402);
		std::string					getDefault_err_page_404();
		void						setDefault_err_page_404(std::string default_err_page_404);
		std::vector<Location>		getLocations();
		// void setLocations(std::list<Location> locations);
		void						addLocation(Location location);
};

#endif
