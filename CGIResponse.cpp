/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 15:19:55 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/01 20:38:00 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIResponse.hpp"
#include <iostream>
#include "Util.hpp"
#include <cstdlib>

CGIResponse::CGIResponse(void) {

	return ;
}

CGIResponse::~CGIResponse(void) {

	return ;
}

void				CGIResponse::parseCGIResponse(std::string const &str) {

	std::size_t			pos;
	std::string			line;
	std::istringstream	iss(str);

	while(std::getline(iss, line))
	{
		if ((pos = line.find("\r")) != std::string::npos)
			line = line.substr(0, pos);
		if (Util::isKey(line, "Content-type"))
			this->_contentType = Util::getValue(line, "Content-type");
		else if (Util::isKey(line, "Location"))
			this->_location = Util::getValue(line, "Location");
		else if (Util::isKey(line, "Status"))
		{
			this->_status = Util::getValue(line, "Status");
			if ((pos = this->_status.find(" ")) != std::string::npos)
			{
				this->_statusCode = atoi(this->_status.substr(0, pos).c_str());
				this->_statusText = this->_status.substr(pos + 1);
			}
		}
		else if (line.empty())
		{
			while (std::getline(iss, line))
				this->_body += line + "\n";
			this->_body = this->_body.substr(0, this->_body.size() - 1); // deletes last '\n'
			// I suppose that there will be no chunked CGI Responses
			break ;
		}
	}
}

std::string const	&CGIResponse::getContentType(void) const {

	return (this->_contentType);
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

void				CGIResponse::setContentType(std::string const &contentType) {

	this->_contentType = contentType;
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
