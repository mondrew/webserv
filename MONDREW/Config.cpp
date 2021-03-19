/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 10:27:07 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/19 12:06:14 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Location.hpp"

Config::Config(std::string const &a_path, EventSelector *a_selector) :
													_path(a_path),
													_the_selector(a_selector) {
	return ;
}

Config::Config(Config const &cp)
{
	*this = cp;
}

Config::~Config()
{
	delete this->_the_selector;
	// _serverSet will be deleted in _the_selector destructor
	// (as instance of ASocketOwner class)
}

Config		&Config::operator=(Config const &op)
{
	this->_serverSet = op._serverSet;
	return (*this);
}

void		Config::addServer(Server *server)
{
	this->_serverSet.push_back(server);
}

Location	*createLocation(std::string const &locationPath, std::string const &root,
													std::string const &index)
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
	Location	*location = new Location();
	location->setLocationPath(locationPath);
	if (!root.empty())
		location->setRoot(root);
	if (!index.empty())
		location->setIndex(index);

	return location;
}

Server		*parseServer(std::string const &serverName, int port)
{
	Server	*server = new Server(-1);

	//server->setSelector(getSelector());
	server->setHost("127.0.0.1");
	server->addServerName(serverName);
	server->setPort(port);
	server->setDefaultErrorPage402("./www/error.html");
	server->setDefaultErrorPage404("./www/error.html");
	server->addLocation(createLocation("", "./www","")); // locationPath | root | index
	server->addLocation(createLocation("test", "./www","")); // locationPath | root | index
	return server;
}

void		Config::createConfig()
{
	addServer(parseServer("localhost", 8000));
	addServer(parseServer("localhost", 8001));
	//You can add more servers. All values is default, except serverName and port.
}

bool		Config::isValid(void) {

	// Check if the configuration object made from webserv.conf is valid
	return (true);
}

void		Config::runServers(void) {

	// Start servers
	for (std::list<Server *>::iterator it = _serverSet.begin();
													it != _serverSet.end(); it++)
		(*it)->start();

	// Run Main Loop
	_the_selector->run();
}

// Getters
std::list<Server *> const	&Config::getServerSet() const {

	return (this->_serverSet);
}

std::string const			&Config::getPath(void) const {

	return (_path);
}

EventSelector				*Config::getSelector(void) const {

	return (_the_selector);
}
