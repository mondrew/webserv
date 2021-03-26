/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 10:27:07 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/26 10:10:31 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <iostream>
#include <fstream>

Config::Config(std::string const &a_path, EventSelector *a_selector) :
													_path(a_path),
													_the_selector(a_selector),
													_isValid(true) {
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

Config			&Config::operator=(Config const &op)
{
	this->_serverSet = op._serverSet;
	return (*this);
}

void			Config::setError(std::string const &error)
{
	this->_error = error;
	this->_isValid = false;
}

void			Config::addServer(Server *server)
{
	this->_serverSet.push_back(server);
}

Location				*createLocation(std::string const &locationPath,
							std::string const &root, std::string const &index)
{
	Location	*location = new Location();
	location->setLocationPath(locationPath);
	if (!root.empty())
		location->setRoot(root);
	if (!index.empty())
		location->setIndex(index);
	return location;
}

int						clearBreak(std::string &str)
{
	if (Util::getLastChar(str) == ';')
	{
		str = str.substr(0, str.size() - 1);
		return (1);
	}
	return (0);
}

Location 		*Config::parseLocation(std::ifstream &file, std::string &partStr)
{
	long 		partLong = 0;
	int 		isBreak = 0;
	Location 	*location = new Location();

	location->setLocationPath(partStr);
	while (!file.eof() && partStr != "}")
	{
		file >> partStr;
		if (partStr == "root")
		{
			file >> partStr;
			clearBreak(partStr);
			location->setRoot(partStr);
		}
		else if (partStr == "client_max_body_size")
		{
			file >> partLong;
			if (partLong <= 0)
			{
				file.close();
				this->setError("Wrong client_max_body_size: " + \
													Util::toString(partLong));
				return (0);
			}
			location->setMaxBody(partLong);
		}
		else if (partStr == "limit_except")
		{
			file >> partStr;
			isBreak = 0;
			while (!file.eof() && !isBreak)
			{
				isBreak = clearBreak(partStr);
				if (partStr == "GET")
					location->addLimitExcept(GET);
				else if (partStr == "POST")
					location->addLimitExcept(POST);
				else if (partStr == "PUT")
					location->addLimitExcept(PUT);
				else if (partStr == "HEAD")
					location->addLimitExcept(HEAD);
				else {
					file.close();
					this->setError("Unknown limit_except type " + partStr);
					return (0);
				}
				file >> partStr;
			}
		}
		else if (partStr == "index")
		{
			file >> partStr;
			clearBreak(partStr);
			location->setIndex(partStr);
		}
		else if (partStr == "autoindex")
		{
			file >> partStr;
			clearBreak(partStr);
			if (partStr != "on" && partStr != "off")
			{
				file.close();
				this->setError("Unknown autoindex type " + partStr);
				return (0);
			}
			location->setAutoindex(partStr == "on");
		}
		else if (partStr == "cgi")
		{
			file >> partStr;
			clearBreak(partStr);
			location->setCgi(partStr);
		}
		else if (partStr == "cgi_path")
		{
			file >> partStr;
			clearBreak(partStr);
			location->setCgiPath(partStr);
		}
	}
	return location;
}

Server 			*Config::parseServer(std::ifstream &file, std::string &partStr)
{

	int 		partInt;
	int 		isBreak = 0;
	Server 		*server = new Server(-1);

	file >> partStr;
	while (!file.eof() && partStr != "}")
	{
		if (partStr == "server_name")
		{
			isBreak = 0;
			file >> partStr;
			while (!isBreak)
			{
				isBreak = clearBreak(partStr);
				server->addServerName(partStr);
				server->setHost(partStr);
				if (isBreak)
					break;
				file >> partStr;
			}
		}
		else if (partStr == "listen")
		{
			isBreak = 0; // ?
			file >> partInt;
			if (partInt <= 0 || partInt > 65535)
			{
				file.close();
				this->setError("Wrong port " + Util::toString(partInt));
				return (0);
			}
			server->setPort(partInt);
		}
		else if (partStr == "error_page")
		{
			isBreak = 0; // no need here
			file >> partInt;
			if (partInt == 402 || partInt == 404)
			{
				file >> partStr;
				clearBreak(partStr);
				if (partInt == 402)
					server->setDefaultErrorPage402(partStr);
				else
					server->setDefaultErrorPage404(partStr);
			}
		}
		else if (partStr == "location")
		{
			file >> partStr;
			Location *location = parseLocation(file, partStr);
			if (!location)
				return (0);
			location->print(); // debug
			server->addLocation(location);
		}
		else if (!file.eof() && partStr != ";")
		{
			file.close();
			this->setError("Unknown config parameter \"" + partStr + "\"");
			return (0);
		}
		file >> partStr;
	}
	return server;
}

int				Config::parseConfig()
{
	std::ifstream	file(this->_path.c_str());
	std::string		partStr;

	if (!file.is_open())
		return Logger::e(ERROR_CONFIG_PARSE);
	while (!file.eof())
	{
		file >> partStr;
		if (partStr == "server") // mon: "server{" fails!
		{
			file >> partStr;
			Server *server = parseServer(file, partStr);
			if (!server)
				return (1);
			server->setSelector(this->getSelector());
			addServer(server);
		}
		else if (!file.eof())
		{
			file.close();
			this->setError("Unknown config parameter \"" + partStr + "\"");
			return 1;
		}
	}
	file.close();
	return (0);
}

bool					Config::isValid(void) {
	return (this->_isValid);
}

std::string	const		&Config::getError(void) const{
	return (this->_error);
}

void					Config::runServers(void) {

	// Start servers
	for (std::list<Server *>::iterator it = _serverSet.begin();
													it != _serverSet.end(); it++)
	{
		(*it)->start();
		_the_selector->add(*it);
	}
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
