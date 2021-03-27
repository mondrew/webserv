/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/27 22:48:56 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "Util.hpp"
#include <vector>
#include <string>
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

bool		Session::checkRequestTarget(void) {

	bool			isValidPath = false;
	std::string		pathFolder;
	std::string		pathFile;

	for (std::vector<Location *>::iterator it = \
								this->_theMaster->getLocationSet().begin();
									it < this->_theMaster->getLocationSet().end(); it++)
	{
		pathFolder = (*it)->getRoot() + (*it)->getLocationPath();
		pathFile = pathFolder + (*it)->getIndex();

		if (Util::getLastChar(this->_request->getTarget()) == '/' && \
							this->_request->getTarget().compare("." + pathFolder) == 0)
		{
			// Here we check if the request target was the folder
			// (for POST method for example)
			isValidPath = true;
			break ;
		}
		else if (this->_request->getTarget().compare("." + pathFile) == 0)
		{
			// Here we check the Path to the concrete file
			isValidPath = true;
			break ;
		}
	}
	return (isValidPath);
}

void		Session::generateResponse(void) {

	this->_request = new HTTPRequest(_requestStr); // create flag _valid = true by default
	this->_response = new HTTPResponse();

	_response->setProtocolVersion("HTTP/1.1");

	if (!_request->isValid())
	{
		// std::cerr << _request->getError();
		_response->setStatusCode(400);
		_response->setStatusText("Bad Request Error");
		_response->setAllow(0);
		_response->setContentLanguage("");
		_response->setContentLength(0);
		_response->setContentLocation("");
		_response->setContentType("");
		_response->setDate("");
		_response->setLastModified("");
		_response->setLocation("");
		_response->setRetryAfter("");
		_response->setServer("Monica/1.0 (Unix)");
		_response->setTransferEncoding("");
		_response->setWWWAuthenticate("");
		_response->setBody("");
	}
	else if (!checkRequestTarget())
	{
		_response->setStatusCode(404);
		_response->setStatusText("Not Found");
		_response->setAllow(0);
		_response->setContentLanguage("");
		_response->setContentLength(0);
		_response->setContentLocation("");
		_response->setContentType("");
		_response->setDate("");
		_response->setLastModified("");
		_response->setLocation("");
		_response->setRetryAfter("");
		_response->setServer("Monica/1.0 (Unix)");
		_response->setTransferEncoding("");
		_response->setWWWAuthenticate("");
		_response->setBody("");
	}
	else
	{
		// std::cout << "START - Parser testing - HTTPRequest\n";
		// _request->print();
		// std::cout << "END - Parser testing - HTTPRequest\n\n"; 
		// fill the _response
		if (_request->getMethod() == GET)
		{
			// return file or run script
			_response->setStatusCode(200);
			_response->setStatusText("OK");
	// _response->_allow = 

		}
		else if (_request->getMethod() == POST)
		{
			// update some info?
			_response->setStatusCode(201);
			_response->setStatusText("Created");
		}
		else if (_request->getMethod() == PUT)
		{
			// Save file to ./www/upload
			_response->setStatusCode(200);
			_response->setStatusText("OK");
		}
		else if (_request->getMethod() == HEAD)
		{
			// do smth
			_response->setStatusCode(200);
			_response->setStatusText("OK");
		}
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
