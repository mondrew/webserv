/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 13:39:10 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/29 15:05:17 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_REQUEST_HPP
# define CGI_REQUEST_HPP

# include "HTTPRequest.hpp"
# include "Session.hpp"

class HTTPRequest;

class CGIRequest {

	private:

		Session			*_session;
		HTTPRequest		*_httpRequest;

		std::string		_authType;
		std::string		_contentLength;
		std::string		_contentType;
		std::string		_gatewayInterface;
		std::string		_pathInfo;
		std::string		_pathTranslated;
		std::string		_queryString;
		std::string		_remoteAddr;
		std::string		_remoteIdent;
		std::string		_remoteUser;
		std::string		_requestMethod;
		std::string		_requestURI;
		std::string		_scriptName;
		std::string		_serverName;
		std::string		_serverPort;
		std::string		_serverProtocol;
		std::string		_serverSoftware;

		CGIRequest(void);
		CGIRequest(CGIRequest const &src);
		CGIRequest		&operator=(CGIRequest const &rhs);

	public:

		CGIRequest(Session *session, HTTPRequest *request);
		~CGIRequest(void);

		void				print(void) const;

		// GETTERS
		std::string const	&getAuthType(void) const;
		std::string const	&getContentLength(void) const;
		std::string const	&getContentType(void) const;
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
		Session				*getSession(void) const;
};

#endif
