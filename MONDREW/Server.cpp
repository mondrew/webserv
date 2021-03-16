/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 09:02:17 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/16 10:18:02 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet.h>

Server::Server(int a_port) : _port(a_port), sockfd(-1) {

	return ;
}

Server::Server(Server const &src) {

	*this = src;
	return ;
}

Server::~Server(void) {

	close(_sockfd);
	return ;
}

Server	&Server::operator=(Server const &rhs) {

	this->_port = rhs._port;
	this->_sockfd = rhs._sockfd;

	return (*this);
}

int		Server::init(void) {

	int					ret;
	int					opt;
	struct sockaddr_in	addr;

	// Creating a server listening socket
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd == -1)
	{
		// Can be exception
		std::cout << "Error: cant't create server socket." << std::endl;
		return (-1);
	}
	std::cout << "Server has created a socket: " << _sockfd << std::endl;

	// Prevents port sticking
	opt = 1;
	setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = htonls(INADDR_ANY);

	// Binding the socket '_sockfd' to IP address
	ret = bind(_sockfd, (struct sockaddr *)&addr, sizeof(addr));
	if (ret == -1)
	{
		// Can be exception
		std::cout << "Error: can't bind IP addressto server port." << std::endl;
		close(_sockfd);
		return (-1);
	}
	std::cout << "Bind successfully" << std::endl;
	std::cout << "Server IP address: " << addr.sin_addr.s_addr << std::endl;

	return (0);
}

void	Server::run(void) {

}
