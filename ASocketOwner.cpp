/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocketOwner.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 00:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/16 23:05:41 by gjessica         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASocketOwner.hpp"
#include <unistd.h>
#include <iostream>

ASocketOwner::ASocketOwner(void) :	_socket(-1),

									_wantToRead(true),
									_wantToWrite(false){

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

	// if (_socket != -1)
	// {
	// 	close(_socket);
	// 	std::cout << "CLOSE SOCKET OWNER " << _socket << "\n";
	// }
	/*
	if (_readFd != -1)
		close(_readFd);
	if (_writeFd != -1)
		close(_writeFd);
	*/
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

// int				ASocketOwner::getReadFd(void) const {
// 	return (_readFd);
// }

// int				ASocketOwner::getWriteFd(void) const {
// 	return (_writeFd);
// }

bool			ASocketOwner::getWantToRead(void) const {

	return (_wantToRead);
}

bool			ASocketOwner::getWantToWrite(void) const {

	return (_wantToWrite);
}

// bool			ASocketOwner::getWantToReadFromFd(void) const {

// 	return (_wantToReadFromFd);
// }

// bool			ASocketOwner::getWantToWriteToFd(void) const {

// 	return (_wantToWriteToFd);
// }

// Setters
void			ASocketOwner::setSocket(int socket) {
	this->_socket = socket;
}

// void			ASocketOwner::setReadFd(int fd) {
// 	this->_readFd = fd;
// }

// void			ASocketOwner::setWriteFd(int fd) {
// 	this->_writeFd = fd;
// }

void			ASocketOwner::setWantToRead(bool val) {

	this->_wantToRead = val;
}

void			ASocketOwner::setWantToWrite(bool val) {

	this->_wantToWrite = val;
}

// void			ASocketOwner::setWantToReadFromFd(bool val) {

// 	this->_wantToReadFromFd = val;
// }

// void			ASocketOwner::setWantToWriteToFd(bool val) {

// 	this->_wantToWriteToFd = val;
// }
