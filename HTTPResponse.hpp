/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 16:49:22 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/20 17:44:12 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP

#include <string>

class HTTPResponse {

	private:

		// STATUS LINE
		std::string		_protocolVersion;
		int				_statusCode;
		std::string		_statusText;

		// HEADERS
		std::string		_accessControlAllowOrigin;
		std::string		_connection;
		std::string		_contentEncoding;
		std::string		_contentType;
		std::string		_date;
		std::string		_eTag;
		std::string		_keepAlive;
		std::string		_lastModified;
		std::string		_server;
		std::string		_setCookie;
		std::string		_transferEncoding;
		std::string		_vary;
		std::string		_xFrameOptions;

		// BODY // not mandatory
		std::string		_body; // or _path

		HTTPResponse(HTTPResponse const &src);
		HTTPResponse	&operator=(HTTPResponse const &rhs);

	public:

		HTTPResponse(void);
		~HTTPResponse(void);

};

#endif
