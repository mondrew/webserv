/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/26 19:23:15 by gjessica         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <algorithm>

Session::Session(int a_sockfd, Server *master) :	ASocketOwner(a_sockfd),
													_theMaster(master),
													_request(0),
													_response(0),
													_deleteMe(false) {
	return ;
}

Session::~Session(void) {

	if (_request)
		delete _request;
	if (_response)
		delete _response;
	return ;
}

bool		Session::getDeleteMe(void) const {

	return (this->_deleteMe);
}

void		Session::generateResponse(void) {

	_request = new HTTPRequest(_requestStr); // create flag _valid = true by default
	_response = new HTTPResponse();

	if (!_request->isValid())
	{
		std::cerr << _request->getError();
		// Here we can limit parameter names, allowed methods, etc
		// 400 Bad Request Error -> fill the _response
	}
	else
	{
		std::cout << "START - Parser testing - HTTPRequest\n";
		_request->print();
		std::cout << "END - Parser testing - HTTPRequest\n\n";
		/*
		// fill the _response
		if (_request->getMethod() == GET)
		{
			// return file or run script
		}
		else if (_request->getMethod() == POST)
		{
			// update some info?
		}
		else if (_request->getMethod() == PUT)
		{
			// Save file to ./www/upload
		}
		else if (_request->getMethod() == HEAD)
		{
			// do smth
		}
		*/
	}
	responseToString();
}

void		Session::responseToString(void) {

	// code

	// just for testing
	_responseStr = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: \
					 									16\n\n<h1>testing</h1>";
}

void		Session::remove(void) {

	_theMaster->removeSession(this);
}

// MONDREW handle
void		Session::handle(void) {

	int		ret;

	if (getWantToRead() == true)
	{
		ret = read(_socket, _buf, BUFFER_SIZE);
		if (ret < 0)
		{
			// Exceptions will be better!
		}
		else if (ret == 0 || ret < BUFFER_SIZE)
		{
			// EOF reached
			_buf[ret] = '\0';
			_requestStr += _buf;
			std::cout << _requestStr << std::endl; // debug
			generateResponse();
			setWantToRead(false);
			setWantToWrite(true);
		}
		else
		{
			_buf[ret] = '\0';
			_requestStr += _buf;
		}
	}
	else if (getWantToWrite() == true)
	{
		if (!_responseStr.empty())
		{
			ret = write(_socket, _responseStr.c_str(), \
				std::min(BUFFER_SIZE, static_cast<int>(_responseStr.length())));
			_responseStr.erase(0, ret);
		}
		else
		{
			// We have sent all HTTP Response
			// Now we have to close the Session
			// HTTP is connectionless protocol !
			_deleteMe = true;
		}
	}
}
