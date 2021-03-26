/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 15:53:09 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/26 19:25:37 by gjessica         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

int	HTTPRequest::setError(std::string str)
{
	this->_error = str;
	this->_valid = false;
	return (1);
}

int 		isKey(std::string line, std::string key)
{
	return (line.find(key) == 0);
}

std::string getValue(std::string line, std::string key)
{
	return line.substr(key.length() + 2);
}

bool HTTPRequest::setStartLineParam(std::string line)
{
	if (isKey(line, "GET"))
		this->_method = GET;
	else if (isKey(line, "POST"))
		this->_method = POST;
	else if (isKey(line, "PUT"))
		this->_method = PUT;
	else
		return setError("Unknown method");

	line = line.substr(line.find("/"));
	this->_target = line.substr(0, line.find(" "));
	this->_protocolVersion = line.substr(line.find(" ") + 1);

	if (this->_target.empty() || this->_protocolVersion.empty())
		return false;
	return (true);
}

void HTTPRequest::parseRequest(std::string str)
{
	std::istringstream f(str);
	std::string line;

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
			this->_contentLength = atoi( getValue(line, "Content-Length").c_str());
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
			break;
		}
    }
}

HTTPRequest::HTTPRequest(std::string const &str) :
												_valid(true),
												_allow(0)
{
	parseRequest(str);
}

HTTPRequest::~HTTPRequest(void) {

	return ;
}

void			HTTPRequest::print(void)
{
	std::cout << "Methods = ";
	if (this->_method == 0)
		std::cout << "default\n";
	else
	{
		if (this->_method & GET) std::cout << "GET";
	 	if (this->_method & POST) std::cout << " POST";
	 	if (this->_method & PUT) std::cout << " PUT";
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

bool			HTTPRequest::isValid(void) {

	// check if request is valid
	return (this->_valid);
}

std::string		HTTPRequest::getError(void)
{
	return (this->_error);
}
