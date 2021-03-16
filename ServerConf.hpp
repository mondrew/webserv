#ifndef SERVER_CONF_HPP
#define SERVER_CONF_HPP

#include <iostream>
#include <list>
#include "Location.hpp"

class ServerConf
{
	private:
		std::string server_name;
		std::string portListen;
		std::string default_err_page_402;
		std::string default_err_page_404;
		std::vector<Location> locations;
	public:
		ServerConf();
		ServerConf(ServerConf const &cp);
		virtual ~ServerConf();
		ServerConf &operator=(ServerConf const &cp);

		std::string getServer_name();
		void setServer_name(std::string server_name);
		std::string getPortListen();
		void setPortListen(std::string portListen);
		std::string getDefault_err_page_402();
		void setDefault_err_page_402(std::string default_err_page_402);
		std::string getDefault_err_page_404();
		void setDefault_err_page_404(std::string default_err_page_404);
		std::vector<Location> getLocations();
		// void setLocations(std::list<Location> locations);
		void addLocation(Location location);
};

#endif
