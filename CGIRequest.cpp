/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 13:59:12 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/19 07:43:48 by mondrew          ###   ########.fr       */
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
	this->_authType += this->_httpRequest->getAuthorizationType;
	if (!this->_httpRequest->getBodyLength())
	{
		this->_contentLength += "";
		this->_contentType += "";
	}
	else
	{
		this->_contentLength += this->_httpRequest->getBodyLength();
		this->_contentType += this->_httpRequest->getContentType();
	}
	this->_gatewayInterface += "CGI/1.1";
	this->_pathInfo += this->_httpRequest->getCgiPathInfo();
	this->_pathTranslated += this->_httpRequest->getCgiPathTranslated();
	this->_queryString += this->_httpRequest->getQueryString();
	this->_remoteAddr += \
				Util::itoips(this->_httpRequest->getSession()->getRemoteAddr());
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
		this->_remoteIdent += user;
		this->_remoteUser += user;
	}
	else
	{
		this->_remoteIdent += "";
		this->_remoteUser += "";
	}

	this->_requestMethod += this->_httpRequest->getMethodName(); 
	this->_requestURI += this->_httpRequest->getTarget();
	this->_scriptName += this->_httpRequest->getTarget();

	this->_serverName += \
			Util::getServerNameFromHost(this->_httpRequest->getHost());
	this->_serverPort += \
			Util::getServerPortFromHost(this->_httpRequest->getHost());
	this->_serverProtocol += this->_httpRequest->getProtocolVersion();
	this->_serverSoftware += "MGINX Webserv/1.0 (Unix)";
}

CGIRequest::~CGIRequest(void) {

	return ;
}

std::string const	&CGIRequest::getAuthType(void) const {

	return (this->_authType);
}

std::string const	&CGIRequest::getContentLength(void) const {

	return (this->_contentLength);
}

std::string const	&CGIRequest::getGatewayInterface(void) const {

	return (this->_gatewayInterface);
}

std::string const	&CGIRequest::getPathInfo(void) const {

	return (this->_pathInfo);
}

std::string const	&CGIRequest::getPathTranslated(void) const {

	return (this->_pathTranslated);
}

std::string const	&CGIRequest::getQueryString(void) const {

	return (this->_queryString);
}

std::string const	&CGIRequest::getRemoteAddr(void) const {

	return (this->_remoteAddr);
}

std::string const	&CGIRequest::getRemoteIdent(void) const {

	return (this->_remoteIdent);
}

std::string const	&CGIRequest::getRemoteUser(void) const {

	return (this->_remoteUser);
}

std::string const	&CGIRequest::getRequestMethod(void) const {

	return (this->_requestMethod);
}

std::string const	&CGIRequest::getRequestURI(void) const {

	return (this->_requestURI);
}

std::string const	&CGIRequest::getScriptName(void) const {

	return (this->_scriptName);
}

std::string const	&CGIRequest::getServerName(void) const {

	return (this->_serverName);
}

std::string const	&CGIRequest::getServerPort(void) const {

	return (this->_serverPort);
}

std::string const	&CGIRequest::getServerProtocol(void) const {

	return (this->_serverProtocol);
}

std::string const	&CGIRequest::getServerSoftware(void) const {

	return (this->_serverSoftware);
}
