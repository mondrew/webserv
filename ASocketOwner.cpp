/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocketOwner.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 00:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/23 15:16:40 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASocketOwner.hpp"
#include <unistd.h>
#include <iostream>

ASocketOwner::ASocketOwner(void) :	_socket(-1),
									_fdCGIRequest(-1),
									_fdCGIResponse(-1),
									_wantToRead(true),
									_wantToWrite(false),
									_wantToWriteCGIRequest(false),
									_wantToReadCGIResponse(false)
{
}

ASocketOwner::ASocketOwner(int a_socket) :	_socket(a_socket),
											_fdCGIRequest(-1),
											_fdCGIResponse(-1),
											_wantToRead(true),
											_wantToWrite(false),
											_wantToWriteCGIRequest(false),
											_wantToReadCGIResponse(false)
{
}

ASocketOwner::ASocketOwner(ASocketOwner const &src)
{
	*this = src;
}

ASocketOwner::~ASocketOwner(void)
{
}

ASocketOwner &ASocketOwner::operator=(ASocketOwner const &rhs)
{
	this->_socket = rhs._socket;
	this->_fdCGIRequest = rhs._fdCGIRequest;
	this->_fdCGIResponse = rhs._fdCGIResponse;

	this->_wantToRead = rhs._wantToRead;
	this->_wantToWrite = rhs._wantToWrite;
	this->_wantToWriteCGIRequest = rhs._wantToWriteCGIRequest;
	this->_wantToReadCGIResponse = rhs._wantToReadCGIResponse;

	return (*this);
}

// Getters
int ASocketOwner::getSocket(void) const
{
	return (_socket);
}

int	ASocketOwner::getFdCGIRequest(void) const
{
	return (_fdCGIRequest);
}

int	ASocketOwner::getFdCGIResponse(void) const
{
	return (_fdCGIResponse);
}

bool ASocketOwner::getWantToRead(void) const
{
	return (_wantToRead);
}

bool ASocketOwner::getWantToWrite(void) const
{
	return (_wantToWrite);
}

bool ASocketOwner::getWantToWriteCGIRequest(void) const
{
	return (_wantToWriteCGIRequest);
}

bool ASocketOwner::getWantToReadCGIResponse(void) const
{
	return (_wantToReadCGIResponse);
}

// Setters
void ASocketOwner::setSocket(int socket)
{
	this->_socket = socket;
}

void ASocketOwner::setFdCGIRequest(int fd)
{
	this->_fdCGIRequest = fd;
}

void ASocketOwner::setFdCGIResponse(int fd)
{
	this->_fdCGIResponse = fd;
}

void ASocketOwner::setWantToRead(bool val)
{
	this->_wantToRead = val;
}

void ASocketOwner::setWantToWrite(bool val)
{
	this->_wantToWrite = val;
}

void ASocketOwner::setWantToWriteCGIRequest(bool val)
{
	this->_wantToWriteCGIRequest = val;
}

void ASocketOwner::setWantToReadCGIResponse(bool val)
{
	this->_wantToReadCGIResponse = val;
}
