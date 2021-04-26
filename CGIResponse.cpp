/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 15:19:55 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/23 20:08:40 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIResponse.hpp"
#include <iostream>
#include "Util.hpp"

CGIResponse::CGIResponse(void) {

	return ;
}

CGIResponse::~CGIResponse(void) {

	return ;
}

void				CGIResponse::parseCGIResponse(std::string const &str) {

	std::size_t		pos;
	std::size_t		posEnd;
	std::string		tmp;

	if ((pos = str.find("Content-Type:")) != std::string::npos || \
			(pos = str.find("Content-type:")) != std::string::npos || \
			(pos = str.find("content-type:")) != std::string::npos)
	{
		tmp = Util::removeLeadingWhitespaces(str.substr(pos + 13));
		if ((posEnd = tmp.find("\n", pos)) == std::string::npos)
		{
			std::cout << "Error occured" << std::endl;
			return ;
		}
		setBody(tmp.substr(posEnd + 1));
		setContentType(tmp.substr(0, posEnd));
	}
}

std::string const	&CGIResponse::getContentType(void) const {

	return (this->_contentType);
}

std::string const	&CGIResponse::getBody(void) const {

	return (this->_body);
}

void				CGIResponse::setBody(std::string const &body) {

	this->_body = body;
}

void				CGIResponse::setContentType(std::string const &contentType) {

	this->_contentType = contentType;
}
