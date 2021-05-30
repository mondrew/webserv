/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 17:15:24 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/29 17:22:35 by mondrew          ###   ########.fr       */
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

void		HTTPResponse::setAllow(char allow) {

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

// GETTERS
int					HTTPResponse::getStatusCode(void) const {

	return (this->_statusCode);
}

int					HTTPResponse::getContentLength(void) const {

	return (this->_contentLength);
}

std::string const	&HTTPResponse::getContentType(void) const {

	return (this->_contentType);
}

char				HTTPResponse::getAllow(void) const {

	return (this->_allow);
}

std::string const	&HTTPResponse::getProtocolVersion(void) const {

	return (this->_protocolVersion);
}

std::string const	&HTTPResponse::getStatusText(void) const {

	return (this->_statusText);
}

std::string const	&HTTPResponse::getContentLanguage(void) const {

	return (this->_contentLanguage);
}

std::string const	&HTTPResponse::getContentLocation(void) const {

	return (this->_contentLocation);
}

std::string const	&HTTPResponse::getDate(void) const {

	return (this->_date);
}

std::string const	&HTTPResponse::getLastModified(void) const {

	return (this->_lastModified);
}

std::string const	&HTTPResponse::getLocation(void) const {

	return (this->_location);
}

std::string const	&HTTPResponse::getRetryAfter(void) const {

	return (this->_retryAfter);
}

std::string const	&HTTPResponse::getServer(void) const {

	return (this->_server);
}

std::string const	&HTTPResponse::getTransferEncoding(void) const {

	return (this->_transferEncoding);
}

std::string const	&HTTPResponse::getWWWAuthenticate(void) const {

	return (this->_WWWAuthenticate);
}

std::string const	&HTTPResponse::getBody(void) const {

	return (this->_body);
}
