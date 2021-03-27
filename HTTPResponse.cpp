/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 17:15:24 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/27 11:03:30 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse(void) {

	return ;
}

HTTPResponse::~HTTPResponse(void) {

	return ;
}

void		HTTPResponse::setProtocolVersion(std::string const &version) {

	this->_protocolVersion = version;
}

void		HTTPResponse::setStatusCode(int code) {

	this->_statusCode = code;
}

void		HTTPResponse::setStatusText(std::string const &text) {

	this->_statusText = text;
}

void		HTTPResponse::setAllow(Options allow) {

	this->_allow = allow;
}

void		HTTPResponse::setContentLanguage(std::string const &language) {

	this->_contentLanguage = language;
}

void		HTTPResponse::setContentLength(int length) {

	this->_contentLength = length;
}

void		HTTPResponse::setContentLocation(std::string const &location) {

	this->_contentLocation = location;
}

void		HTTPResponse::setContentType(std::string const &type) {

	this->_contentType = type;
}

void		HTTPResponse::setDate(std::string const &date) {

	this->_date = date;
}

void		HTTPResponse::setLastModified(std::string const &date) {

	this->_lastModified = date;
}

void		HTTPResponse::setLocation(std::string const &location) {

	this->_location = location;
}

void		HTTPResponse::setRetryAfter(std::string const &retry) {

	this->_retryAfter = retry;
}

void		HTTPResponse::setServer(std::string const &server) {

	this->_server = server;
}

void		HTTPResponse::setTransferEncoding(std::string const &encoding) {

	this->_transferEncoding = encoding;
}

void		HTTPResponse::setWWWAuthenticate(std::string const auth) {

	this->_WWWAuthenticate = auth;
}

void		HTTPResponse::setBody(std::string const &body) {

	this->_body = body;
}
