/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 13:59:12 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/17 08:50:30 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIRequest.hpp"
#include <string>

CGIRequest::CGIRequest(void) : _httpRequest(0) {

		_authType = "AUTH_TYPE=";
		_contentLength = "CONTENT_LENGTH=";
		_gatewayInterface = "GATEWAY_INTERFACE=";
		_pathInfo = "PATH_INFO=";
		_pathTranslated = "PATH_TRANSLATED";
		_queryString = "QUERY_STRING=";
		_remoteAddr = "REMOTE_ADDR=";
		_remoteIdent = "REMOTE_IDENT=";
		_remoteUser = "REMOTE_USER=";
		_requestMethod = "REQUEST_METHOD=";
		_requestURI = "REQUEST_URI=";
		_scriptName = "SCRIPT_NAME=";
		_serverName = "SERVER_NAME=";
		_serverPort = "SERVER_PORT=";
		_serverProtocol = "SERVER_PROTOCOL=";
		_serverSoftware = "SERVER_SOFTWARE=";
}

CGIRequest::CGIRequest(HTTPRequest *request) : CGIRequest(),
												_httpRequest(request) {

	// The Script-URI
	this->_authType += "\"" + this->_httpRequest->getAuthorizationType + "\"";
	if (!this->_httpRequest->getBodyLength())
	{
		this->_contentLength += "\"\"";
		this->_contentType += "\"\"";
	}
	else
	{
		this->_contentLength += "\"" + \
									 this->_httpRequest->getBodyLength() + "\"";
		this->_contentType += "\"" + \
								   this->_httpRequest->getContentType() + "\"";
	}
	this->_gatewayInterface += "\"CGI/1.1\"";
	this->_pathInfo += "\"" + this->_httpRequest->getCgiPathInfo() + "\"";
	this->_pathTranslated += "\"" + \
							  this->_httpRequest->getCgiPathTranslated() + "\"";
	this->_queryString += "\"" + this->_httpRequest->getQueryString() + "\"";
	this->_remoteAddr += "\"" + \
		Util::itoips(this->_httpRequest->getSession()->getRemoteAddr()) + "\"";
	this->_remoteIdent += "\"\""; // RFC 931 Indentification Protocol ? Is it necessary?
	if (!this->_authType.empty())
	{
		std::string		user;
		std::string		data = \
			Util::decodeBase64(this->_httpRequest->getAuthorizationData());
		std::size_t		found = data.find(":");

		if (found == std::string::npos)
			user = "";
		else
			user = data.substr(0, found);
		this->_remoteUser += "\"" + user + "\"";
	}
	else
		this->_remoteUser += "\"\"";

	this->_requestMethod += "\"" + this->_httpRequest->getMethod() + "\"";

	setServerName(this->_httpRequest->getHost());
	setServerPort(this->_httpRequest->getHost());
	this->_serverProtocol += "\"" + this->_httpRequest->getProtocolVersion() + "\"";
}

CGIRequest::~CGIRequest(void) {

	return ;
}

/*
std::string	CGIRequest::findPathInfo(void) {

	std::string		tmp;

	if (!Util::isPathInfo(this->_httpRequest->getTarget()))
		return ("");
	else
	{
		std::size_t		found;

		if ((found = this->_httpRequest->getTarget().find("cgi-bin")) != \
															std::string::npos)
		{
			tmp = this->_httpRequest->getTarget().substr(found + 8);
			if ((found = tmp.find("/")) != std::string::npos)
				return (tmp.substr(found));
			else
				return ("");
		}
		else
			return ("");
	}
}

std::string	CGIRequest::findPathTranslated(void) {

	if (this->_pathInfo.empty())
		return ("");
	else
	{
		return (this->_httpRequest->_session->_serverLocation->getRoot() + \
															this->_pathInfo);
	}
}
*/

void		CGITRequest::setServerName(std::string const &host) {

	std::size_t		found = host.find(":");

	if (found == std::string::npos)
		this->_serverName = host;
	else
		this->_serverName = host.substr(0, found);
}

void		CGIRequest::setServerPort(std::string const &host) {

	std::size_t		found = host.find(":");

	if (found == std::string::npos)
		this->_serverPort = "80";
	else
		this->_serverPort = host.substr(found);
}
