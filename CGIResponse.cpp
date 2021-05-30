/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 15:19:55 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/29 15:10:33 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIResponse.hpp"
#include <iostream>
#include "Util.hpp"
#include <cstdlib>
#include <stdlib.h>

CGIResponse::CGIResponse(void) {

	_contentType = "";
	_location = "";
	_status = "";
	_statusCode = 0;
	_statusText = "";
	_contentLength = 0;
	_body = "";
}

CGIResponse::~CGIResponse(void) {

	return ;
}

void				CGIResponse::parseCGIResponse(std::string &str) {

	std::size_t			pos;
	std::string			line;
	std::istringstream	iss(str);
	int					lineSize = 0;

	while(std::getline(iss, line))
	{
		lineSize = line.length() + 1;
		if ((pos = line.find("\r")) != std::string::npos)
			line = line.substr(0, pos);
		if (Util::isKey(line, "Content-type"))
			this->_contentType = Util::getValue(line, "Content-type");
		else if (Util::isKey(line, "Location"))
			this->_location = Util::getValue(line, "Location");
		else if (Util::isKey(line, "Content-Length"))
			this->_contentLength = atoi(Util::getValue(line, "Content-Length").c_str());
		else if (Util::isKey(line, "Status"))
		{
			this->_status = Util::getValue(line, "Status");
			if ((pos = this->_status.find(" ")) != std::string::npos)
			{
				this->_statusCode = atoi(this->_status.substr(0, pos).c_str());
				if (this->_status.length() > pos + 1)
					this->_statusText = this->_status.substr(pos + 1);
				else
					this->_statusText = "";
			}
		}
		else if (line.empty())
		{
			str.erase(0, lineSize);
			this->_body = str;
			break ;
		}
		str.erase(0, lineSize);
	}
}

void				CGIResponse::print(void) const {

	std::cout << "_contentType: " << _contentType << std::endl;
	std::cout << "_location: " << _location << std::endl;
	std::cout << "_status: " << _status << std::endl;
	std::cout << "_contentLength: " << _contentLength << std::endl;
}

// GETTERS
std::string const	&CGIResponse::getContentType(void) const {

	return (this->_contentType);
}

int					CGIResponse::getContentLength(void) const {

	return (this->_contentLength);
}

std::string const	&CGIResponse::getLocation(void) const {

	return (this->_location);
}

std::string const	&CGIResponse::getStatus(void) const {

	return (this->_status);
}

int					CGIResponse::getStatusCode(void) const {

	return (this->_statusCode);
}

std::string const	&CGIResponse::getStatusText(void) const {

	return (this->_statusText);
}

std::string const	&CGIResponse::getBody(void) const {

	return (this->_body);
}

// SETTERS
void				CGIResponse::setContentType(std::string const &contentType) {

	this->_contentType = contentType;
}

void				CGIResponse::setContentLength(int contentLength) {

	this->_contentLength = contentLength;
}

void				CGIResponse::setLocation(std::string const &location) {

	this->_location = location;
}

void				CGIResponse::setStatus(std::string const &status) {

	this->_status = status;
}

void				CGIResponse::setBody(std::string const &body) {

	this->_body = body;
}
