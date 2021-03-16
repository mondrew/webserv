/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AFdHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 10:20:18 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/11 21:58:00 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AFdHandler.hpp"
#include <unistd.h>

AFdHandler::AFdHandler(int a_fd, bool own = true) : _fd(a_fd), _own_fd(own) {

	return ;
}

AFdHandler::AFdHandler(AFdHandler const &src) {

	*this = src;
	return ;
}

AFdHandler::~AFdHandler(void) {

	if (_own_fd)
		close(_fd);
	return ;
}

AFdHandler	&AFdHandler::operator=(AFdHandler const &rhs) {

	this->_fd = rhs._fd;
	this->_own_fd = rhs._own_fd;

	return (*this);
}

int			AFdHandler::getFd(void) const {

	return (this->_fd);
}

bool		AFdHandler::wantRead(void) const {

	return (true);
}

bool		AFdHandler::wantWrite(void) const {

	return (false);
}
