/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 16:49:22 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/27 22:21:14 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP

#include <string>
#include "Util.hpp"

class HTTPResponse {

	private:

		// STATUS LINE
		std::string		_protocolVersion;
		int				_statusCode; // 200 OK; 201 Created (POST response)
		std::string		_statusText;

		// HEADERS
		// USE THIS vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
		// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers
		// USE THIS ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
		// Subject
		Options			_allow; // depends on Config. For example: GET, POST, PUT, HEAD
								// must be sent with '405 Method Not Allowed' status code
								// can be empty (if there are no allowed methods

		std::string		_contentLanguage; // example: de-DE
										// example: en, de-DE, en-CA

		int				_contentLength; // example: 55

		std::string		_contentLocation; // documents/foo.json

		std::string		_contentType; // example: text/html; charset=UTF-8
									// example: multipart/form-data; boundary=something

		std::string		_date; // example: Wed, 21 Oct 2015 07:28:00 GMT

		std::string		_lastModified; // date and time the resource was last modified
										// example: Wed, 21 Oct 2015 07:28:00 GMT

		std::string		_location; // indicates the URL to redirect a page to
								// only for 3xx (redirection) or 201 (created) status response
								// 301 Permanently Redirect - returns new URL
								// 302 Found
								// 201 Created - returns Location of created resource
								// example: /index.html

		std::string		_retryAfter; // indicates how long the user agent should wait b4 next request
									// It is sent with the following errors:
									// - 503 (Service Unavailable)
									// - 429 (Too Many Requests)
									// - 301 (Moved Permanently)
									// Value can be:
									// - <http-date>
									// - <delay-seconds>
									// example: Wed, 21 Oct 2015 07:28:00 GMT
									// example: 120

		std::string		_server; // name of the webserver program
								// example: Apache/2.4.1 (Unix)
								// example: nginx/1.4.0 (Ubuntu)
								// example: Monica/1.0 (Unix)

		std::string		_transferEncoding; // specifies the form of encoding
										// example: chunked
										// example: compress
										// example: deflate
										// example: gzip
										// example: identity
										// example: gzip, chunked

		std::string		_WWWAuthenticate; // defines authentication method to access the resource
										// It is sent along with '401 Unauthorized'
							// example: Basic realm="Access to the staging site", charset="UTF-8"

		// BODY // not mandatory
		std::string		_body; // or _path

		HTTPResponse(HTTPResponse const &src);
		HTTPResponse	&operator=(HTTPResponse const &rhs);

	public:

		HTTPResponse(void);
		~HTTPResponse(void);

		//SETTERS
		void		setProtocolVersion(std::string const &version);
		void		setStatusCode(int code);
		void		setStatusText(std::string const &text);
		void		setAllow(Options allow);
		void		setContentLanguage(std::string const &language);
		void		setContentLength(int length);
		void		setContentLocation(std::string const &location);
		void		setContentType(std::string const &type);
		void		setDate(std::string const &date);
		void		setLastModified(std::string const &date);
		void		setLocation(std::string const &location);
		void		setRetryAfter(std::string const &retry);
		void		setServer(std::string const &server);
		void		setTransferEncoding(std::string const &encoding);
		void		setWWWAuthenticate(std::string const auth);
		void		setBody(std::string const &body);
};

#endif
