/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 09:02:17 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/22 12:01:11 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>

#define MAX_USERS 128

Server::Server(int a_socket) : ASocketOwner(a_socket),
					_the_selector(0),
					_serverNames(std::vector<std::string>()),
					_host("localhost"),
					_port(80),
					_defaultErrorPage402(""),
					_defaultErrorPage404(""),
					_locationSet(std::vector<Location *>()),
					_sessionSet(std::list<Session *>()) {
					//_readyToResponseFlag(false) {

	return ;
}

Server::Server(Server const &src) : ASocketOwner(src) {

	*this = src;
	return ;
}

Server::~Server(void) {

	// delete sessionSet ?
	// delete locationSet ?
	for (std::list<Session *>::iterator it = _sessionSet.begin();
												it != _sessionSet.end(); it++)
	{
		_the_selector->remove(*it);
	}
	_the_selector->remove(this);
	return ;
}

Server	&Server::operator=(Server const &rhs) {

	this->_socket = rhs._socket;
	this->_serverNames = rhs._serverNames;
	this->_host = rhs._host;
	this->_port = rhs._port;
	this->_defaultErrorPage402 = rhs._defaultErrorPage402;
	this->_defaultErrorPage404 = rhs._defaultErrorPage404;
	this->_locationSet = rhs._locationSet;
	this->_sessionSet = rhs._sessionSet;

	return (*this);
}

// Getters

// Getters
std::vector<std::string> const	&Server::getServerNames(void) const {

	return (this->_serverNames);
}

std::string const				&Server::getHost(void) const {

	return (this->_host);
}

int								Server::getPort(void) const {

	return (this->_port);
}

std::string const				&Server::getDefaultErrorPage402(void) const {

	return (this->_defaultErrorPage402);
}

std::string const				&Server::getDefaultErrorPage404(void) const {

	return (this->_defaultErrorPage404);
}

std::vector<Location *> const		&Server::getLocationSet(void) const {

	return (this->_locationSet);
}

std::list<Session *> const		&Server::getSessionSet(void) const {

	return (this->_sessionSet);
}

// Setters
void	Server::addServerName(std::string const &serverName) {

	this->_serverNames.push_back(serverName);
}

void	Server::setHost(std::string const &host) {

	this->_host = host;
}

void	Server::setPort(int port) {

	this->_port = port;
}

void	Server::setDefaultErrorPage402(std::string const &path) {

	this->_defaultErrorPage402 = path;
}

void	Server::setDefaultErrorPage404(std::string const &path) {

	this->_defaultErrorPage404 = path;
}

void	Server::addLocation(Location *location) {

	this->_locationSet.push_back(location);
}

void	Server::addSession(Session *session) {

	this->_sessionSet.push_back(session);
}

void	Server::setSelector(EventSelector *selector) {

	this->_the_selector = selector;
}

// Start server
int		Server::start(void) {

	int					ret;
	int					opt;
	struct sockaddr_in	addr;

	// Creating a server listening socket
	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		// Can be exception
		std::cout << "Error: cant't create server socket." << std::endl;
		return (-1);
	}
	std::cout << "Server has created a socket: " << _socket << std::endl;

	// Prevents port sticking
	opt = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		// Can be exception
		std::cout << "Error: can't set socket option 'reusable'" << std::endl;
		close(_socket);
		setSocket(-1);
		return (-1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = inet_addr(_host.c_str());
	//addr.sin_addr.s_addr = htonls(INADDR_ANY);

	// Binding the socket '_listenSocket' to IP address
	if ((ret = bind(_socket, (struct sockaddr *)&addr, sizeof(addr))) == -1)
	{
		// Can be exception
		std::cout << "Error: can't bind IP addressto server port." << std::endl;
		close(_socket);
		setSocket(-1);
		return (-1);
	}
	std::cout << "Bind successfully" << std::endl;
	std::cout << "Server IP address: " << inet_ntoa(addr.sin_addr);
   	std::cout << std::endl;

	// Turn socket to the listening mode
	if (listen(_socket, MAX_USERS) == -1)
	{
		// Can be exception
		std::cout << "Error: can't turn socket to the listening mode" << std::endl;
		close(_socket);
		setSocket(-1);
		return (-1);
	}

	std::cout << "Server '" << _serverNames[0] << "' has successfully started!";
	std::cout << std::endl;

	return (0);
}

void		Server::handle(void) {

	int					sockfd;
	struct sockaddr_in	addr;
	socklen_t			len = sizeof(addr);

	if ((sockfd = accept(_socket, (struct sockaddr *)&addr, &len)) == -1)
	{
		// Can be exception
		std::cout << "Error: can't accept new connection on " << _socket;
		std::cout << " socket." << std::endl;
		close(_socket);
		setSocket(-1);
		return ;
	}

	std::cout << "SERVER ACCEPT: " << sockfd << std::endl; // debug

	// Add new client to the Session list and to the EventSelector objects
	Session	*session = new Session(sockfd, this);

	addSession(session);
	_the_selector->add(session);
}

void		Server::removeSession(Session *s) {

	// Delete Client from the sessionSet linked-list in Server
	_sessionSet.remove(s);

	// Remove fd from EventSelector object array
	_the_selector->remove(s);
}

/*
bool		Server::isReadyToResponse(void) const {

	return (this->_readyToResponseFlag);
}
*/