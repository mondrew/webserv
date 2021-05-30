/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 16:49:22 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/29 17:22:14 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP

# include <string>
# include "Util.hpp"

class HTTPResponse {

	private:

		// STATUS LINE
		std::string		_protocolVersion;
		int				_statusCode;
		std::string		_statusText;

		// HEADERS
		char			_allow;
		std::string		_contentLanguage;
		int				_contentLength;
		std::string		_contentLocation;
		std::string		_contentType;
		std::string		_date; // Wed, 21 Oct 2015 07:28:00 GMT

		std::string		_lastModified;
		std::string		_location;
		std::string		_retryAfter;
		std::string		_server;
		std::string		_transferEncoding;
		std::string		_WWWAuthenticate; // defines authentication method to access the resource
										// It is sent along with '401 Unauthorized'
							// example: Basic realm="Access to the staging site", charset="UTF-8"
		// BODY (not mandatory)
		std::string		_body;

		HTTPResponse(HTTPResponse const &src);
		HTTPResponse	&operator=(HTTPResponse const &rhs);

	public:

		HTTPResponse(void);
		~HTTPResponse(void);

		void				analyzeCGIResponse(std::string const &str);

		// GETTERS
		int					getStatusCode(void) const;
		int					getContentLength(void) const;
		char				getAllow(void) const;
		std::string const	&getProtocolVersion(void) const;
		std::string const	&getStatusText(void) const;
		std::string const	&getContentLanguage(void) const;
		std::string const	&getContentLocation(void) const;
		std::string const	&getContentType(void) const;
		std::string const	&getDate(void) const;
		std::string const	&getLastModified(void) const;
		std::string const	&getLocation(void) const;
		std::string const	&getRetryAfter(void) const;
		std::string const	&getServer(void) const;
		std::string const	&getTransferEncoding(void) const;
		std::string const	&getWWWAuthenticate(void) const;
		std::string const	&getBody(void) const;

		//SETTERS
		void				setProtocolVersion(std::string const &version);
		void				setStatusCode(int code);
		void				setStatusText(std::string const &text);
		void				setAllow(char allow);
		void				setContentLanguage(std::string const &language);
		void				setContentLength(int length);
		void				setContentLocation(std::string const &location);
		void				setContentType(std::string const &type);
		void				setDate(std::string const &date);
		void				setLastModified(std::string const &date);
		void				setLocation(std::string const &location);
		void				setRetryAfter(std::string const &retry);
		void				setServer(std::string const &server);
		void				setTransferEncoding(std::string const &encoding);
		void				setWWWAuthenticate(std::string const auth);
		void				setBody(std::string const &path);
};

#endif
