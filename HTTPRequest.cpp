/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 15:53:09 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/07 12:54:53 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"
#include "Util.hpp"
#include <cstdlib>

HTTPRequest::HTTPRequest(std::string const &str) : _valid(true), _allow(0) {

	parseRequest(str);
}

HTTPRequest::~HTTPRequest(void) {

	return ;
}

bool			HTTPRequest::isValid(void) const
{
	return (this->_valid);
}


int				HTTPRequest::setError(std::string const &str)
{
	this->_error = str;
	this->_valid = false;
	return (1); // Why 1 ? May be 0 ? We need to stop parsing in this case
}

void			HTTPRequest::setBody(std::string const &str) {

	this->_body = str;
}

int 			isKey(std::string const &line, std::string const &key)
{
	return (line.find(key) == 0);
}

std::string		getValue(std::string const &line, std::string const &key)
{
	return line.substr(key.length() + 2);
}

bool 			HTTPRequest::setStartLineParam(std::string &line)
{
	if (isKey(line, "GET"))
		this->_method = GET;
	else if (isKey(line, "POST"))
		this->_method = POST;
	else if (isKey(line, "PUT"))
		this->_method = PUT;
	else if (isKey(line, "HEAD"))
		this->_method = HEAD;
	else
	{
		this->_method = UNKNOWN;
		// We should not set _valid to false here, 'cause we need to response '405' error
		return (false);
		// return setError("Unknown method"); // Here we return true. Why not false?
	}

	line = line.substr(line.find("/"));
	this->_target = line.substr(0, line.find(" "));
	this->_protocolVersion = line.substr(line.find(" ") + 1);

	if (this->_target.empty() || this->_protocolVersion.empty())
		return (false);
	return (true);
}

void			HTTPRequest::parseRequest(std::string const &str)
{
	std::istringstream	f(str);
	std::string			line;

	//Check first line - METHOD PATH PROTOCOL
	std::getline(f, line);
	if (!setStartLineParam(line))
		return ;

    while (std::getline(f, line)) {
		if (isKey(line, "Accept-Charsets"))
			this->_acceptCharset = getValue(line, "Accept-Charsets");
		else if (isKey(line, "Accept-Language"))
			this->_acceptLanguage = getValue(line, "Accept-Language");
		else if (isKey(line, "Allow"))
		{
			if (line.find("GET") == 0)
				this->_allow += GET;
			if (line.find("POST") == 0)
				this->_allow += POST;
			if (line.find("PUT") == 0)
				this->_allow += PUT;
			if (line.find("HEAD") == 0)
				this->_allow += HEAD;
		}
		else if (isKey(line, "Authorization"))
			this->_authorization = getValue(line, "Authorization");
		else if (isKey(line, "Content-Language"))
			this->_contentLanguage = getValue(line, "Content-Language");
		else if (isKey(line, "Content-Length"))
			this->_contentLength = atoi(getValue(line, "Content-Length").c_str());
		else if (isKey(line, "Content-Location"))
			this->_contentLocation = getValue(line, "Content-Location");
		else if (isKey(line, "Content-Type"))
			this->_contentType = getValue(line, "Content-Type");
		else if (isKey(line, "Date"))
			this->_date = getValue(line, "Date");
		else if (isKey(line, "Host"))
			this->_host = getValue(line, "Host");
		else if (isKey(line, "Referer"))
			this->_referer = getValue(line, "Referer");
		else if (isKey(line, "User-Agent"))
			this->_userAgent = getValue(line, "User-Agent");
		else if (line.empty())
		{
			while (std::getline(f, line))
				this->_body += line;
			if (!this->_body.empty() && this->_contentLength == 0)
				this->_body = Util::unchunkData(this->_body);
			break;
		}
    }
}

void			HTTPRequest::print(void) const
{
	std::cout << "Methods = ";
	if (this->_method == UNKNOWN)
	{
		std::cout << "default\n";
	}
	else
	{
		if (this->_method & GET) std::cout << "GET";
	 	if (this->_method & POST) std::cout << "POST";
	 	if (this->_method & PUT) std::cout << "PUT";
	 	if (this->_method & HEAD) std::cout << "HEAD";
		std::cout << std::endl;
	}
	std::cout << "Target = " << this->_target << std::endl;
	std::cout << "Protocol = " << this->_protocolVersion << std::endl;
	std::cout << "Accept-Charsets = " << this->_acceptCharset << std::endl;
	std::cout << "Accept-Language = " << this->_acceptLanguage << std::endl;
	std::cout << "Authorization = " << this->_authorization << std::endl;
	std::cout << "Content-Language = " << this->_contentLanguage << std::endl;
	std::cout << "Content-Length = " << this->_contentLength << std::endl;
	std::cout << "Content-Location = " << this->_contentLocation << std::endl;
	std::cout << "Content-Type = " << this->_contentType << std::endl;
	std::cout << "Date = " << this->_date << std::endl;
	std::cout << "Host = " << this->_host << std::endl;
	std::cout << "Referer = " << this->_referer << std::endl;
	std::cout << "User-Agent = " << this->_userAgent << std::endl;
	std::cout << "Body = " << this->_body << std::endl;
}

std::string const	&HTTPRequest::getError(void) const
{
	return (this->_error);
}

Options				HTTPRequest::getMethod(void) const {

	return (this->_method);
}

std::string const	&HTTPRequest::getTarget(void) const {

	return (this->_target);
}

std::string const	&HTTPRequest::getProtocolVersion(void) const {

	return (this->_protocolVersion);
}

std::string const	&HTTPRequest::getAcceptCharset(void) const {

	return (this->_acceptCharset);
}

std::string const	&HTTPRequest::getAcceptLanguage(void) const {

	return (this->_acceptLanguage);
}

char 				HTTPRequest::getAllow(void) const {

	return (this->_allow);
}

std::string const	&HTTPRequest::getAuthorization(void) const {

	return (this->_authorization);
}

std::string const	&HTTPRequest::getContentLanguage(void) const {

	return (this->_contentLanguage);
}

int					HTTPRequest::getContentLength(void) const {

	return (this->_contentLength);
}

std::string const	&HTTPRequest::getContentLocation(void) const {

	return (this->_contentLocation);
}

std::string const	&HTTPRequest::getcontentType(void) const {

	return (this->_contentType);
}

std::string const	&HTTPRequest::getDate(void) const {

	return (this->_date);
}

std::string const	&HTTPRequest::getHost(void) const {

	return (this->_host);
}

std::string const	&HTTPRequest::getReferer(void) const {

	return (this->_referer);
}

std::string const	&HTTPRequest::getUserAgent(void) const {

	return (this->_userAgent);
}

std::string const	&HTTPRequest::getBody(void) const {

	return (this->_body);
}
