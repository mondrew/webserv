/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocketOwner.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 00:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/30 09:50:23 by gjessica         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASocketOwner.hpp"
#include <unistd.h>

ASocketOwner::ASocketOwner(void) :	_socket(-1),
									_wantToRead(true),
									_wantToWrite(false) {

	return ;
}

ASocketOwner::ASocketOwner(int a_socket) :	_socket(a_socket),
											_wantToRead(true),
											_wantToWrite(false) {

	return ;
}

ASocketOwner::ASocketOwner(ASocketOwner const &src) {

	*this = src;
}

ASocketOwner::~ASocketOwner(void) {

	if (_socket != -1)
		close(_socket);
}

ASocketOwner	&ASocketOwner::operator=(ASocketOwner const &rhs) {

	this->_socket = rhs._socket;
	this->_wantToRead = rhs._wantToRead;
	this->_wantToWrite = rhs._wantToWrite;
	return (*this);
}

// Getters
int				ASocketOwner::getSocket(void) const {
	return (_socket);
}

bool			ASocketOwner::getWantToRead(void) const {

	return (_wantToRead);
}

bool			ASocketOwner::getWantToWrite(void) const {

	return (_wantToWrite);
}

// Setters
void			ASocketOwner::setSocket(int socket) {
	this->_socket = socket;
}

void			ASocketOwner::setWantToRead(bool val) {

	this->_wantToRead = val;
}

void			ASocketOwner::setWantToWrite(bool val) {

	this->_wantToWrite = val;
}
