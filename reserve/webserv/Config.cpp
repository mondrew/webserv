#include "Config.hpp"

Config::Config()
{

}

Config::~Config()
{

}

Config::Config(Config const &cp)
{
	*this = cp;
}

Config & Config::operator=(Config const &op)
{
	this->servers = op.servers;
	return (*this);
}

std::vector<ServerConf> Config::getServers()
{
	return this->servers;
}

void Config::addServer(ServerConf server)
{
	this->servers.push_back(server);
}

Location createLocation(std::string locationPath, std::string root,
															std::string index)
{
	/*
		---------Default---------
		root			./www
		index			index.html
		limit_except	GET POST
		autoindex		on
		max_body		NOT_LIMIT
		cgi				not config
		cgi_index		not config
	*/
	Location location;
	location.setLocationPath(locationPath);
	if (!root.empty()) {location.setRoot(root);}
	if (!index.empty()) {location.setIndex(index);}

	return location;
}

ServerConf parseServer(std::string localhost, std::string port)
{
	ServerConf server;
	server.setServer_name(localhost);
	server.setPortListen(port);
	server.setDefault_err_page_402("./www/error.html");
	server.setDefault_err_page_404("./www/error.html");
	server.addLocation(createLocation("", "./www","")); // locationPath | root | index
	server.addLocation(createLocation("test", "./www","")); // locationPath | root | index
	return server;
}

void Config::createConfig()
{
	addServer(parseServer("localhost", "8000"));
	addServer(parseServer("localhost", "8001"));
	//You can add more servers. All values is default, except serverName and port.
}


