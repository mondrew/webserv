/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 15:48:48 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/22 14:28:49 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include <string>

class HTTPRequest {

	private:

		// https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages

		std::string		_valid; // = true by default
		// START LINE
		std::string		_method; // GET, PUT, POST, HEAD, OPTIONS, CONNECT
		std::string		_target; // URI
		std::string		_ProtocolVersion;

		// HEADERS
		// Request Headers
		std::string		_host;
		std::string		_userAgent;
		std::string		_accept;
		std::string		_acceptLanguage;
		std::string		_acceptEncoding;
		// General Headers
		std::string		_connection;
		int				_upgradeInsecureRequests;
		// Representation headers
		std::string		_contentType;
		int				_contentLength;

		// BODY
		std::string		_body; // or _path

		HTTPRequest(void);
		HTTPRequest(HTTPRequest const &src);
		HTTPRequest		&operator=(HTTPRequest const &rhs);

	public:

		HTTPRequest(std::string const &str);
		~HTTPRequest(void);

		bool			isValid(void);

};

#endif
