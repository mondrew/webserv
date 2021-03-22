/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/22 12:19:09 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"
#include "Server.hpp"
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <algorithm>

Session::Session(int a_sockfd, Server *master) :	ASocketOwner(a_sockfd),
													_theMaster(master),
													_request(0),
													_response(0) {
	return ;
}

Session::~Session(void) {

	if (_request)
		delete _request;
	if (_response)
		delete _response;
	return ;
}

void		Session::generateResponse(void) {

	_request = new HTTPRequest(_requestStr);
	_response = new HTTPResponse();

	if (!_request->isValid())
	{
		// 400 Bad Request Error -> fill the _response
	}
	else
	{
		// fill the _response
	}
	responseToString();
}

void		Session::responseToString(void) {

	// code

	// just for testing
	_responseStr = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: \
					 									16\n\n<h1>testing</h1>";
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
			_theMaster->removeSession(this);
		}
	}
}
