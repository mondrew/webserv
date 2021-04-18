/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 13:39:10 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/17 21:40:04 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_REQUEST_HPP
# define CGI_REQUEST_HPP

class CGIRequest {

	private:

		HTTPRequest		*_httpRequest;

		std::string		authType = "AUTH_TYPE=";
		std::string		contentLength = "CONTENT_LENGTH=";
		std::string		gatewayInterface = "GATEWAY_INTERFACE=";
		std::string		pathInfo = "PATH_INFO=";
		std::string		pathTranslated = "PATH_TRANSLATED";
		std::string		queryString = "QUERY_STRING=";
		std::string		remoteAddr = "REMOTE_ADDR=";
		std::string		remoteIdent = "REMOTE_IDENT=";
		std::string		remoteUser = "REMOTE_USER=";
		std::string		requestMethod = "REQUEST_METHOD=";
		std::string		requestURI = "REQUEST_URI=";
		std::string		scriptName = "SCRIPT_NAME=";
		std::string		serverName = "SERVER_NAME=";
		std::string		serverPort = "SERVER_PORT=";
		std::string		serverProtocol = "SERVER_PROTOCOL=";
		std::string		serverSoftware = "SERVER_SOFTWARE=";

		CGIRequest(void);
		CGIRequest(CGIRequest const &src);
		CGIRequest		&operator=(CGIRequest const &rhs);

	public:

		CGIRequest(HTTPRequest *request);
		~CGIRequest(void);

		// GETTERS
		std::string const	&getAuthType(void) const;
		std::string const	&getContentLength(void) const;
		std::string const	&getGatewayInterface(void) const;
		std::string const	&getPathInfo(void) const;
		std::string const	&getPathTranslated(void) const;
		std::string const	&getQueryString(void) const;
		std::string const	&getRemoteAddr(void) const;
		std::string const	&getRemoteIdent(void) const;
		std::string const	&getRemoteUser(void) const;
		std::string const	&getRequestMethod(void) const;
		std::string const	&getRequestURI(void) const;
		std::string const	&getScriptName(void) const;
		std::string const	&getServerName(void) const;
		std::string const	&getServerPort(void) const;
		std::string const	&getServerProtocol(void) const;
		std::string const	&getServerSoftware(void) const;

};

#endif
