/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 15:48:48 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/26 11:26:38 by mondrew          ###   ########.fr       */
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
								// Question: is there can be several methods??? Do list?
		std::string		_target; // URI
		std::string		_ProtocolVersion;

		// HEADERS
		// USE THIS vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
		// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers
		// USE THIS ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
		// Subject
		std::string		_acceptCharset; // example: utf-8, iso-8859-15;q=0.8 // can be empty
										// SERVER RESPONDS with 'Content-Type' or ignore

		std::string		_acceptLanguage; // example: en, en-US, fr-CA, fr;q=0.9
										// SERVER RESPONDS with 'Content-Language' or ignore

		std::string		_allow; // example: GET, POST, PUT, HEAD
								// can be empty (if there are no allowed methods

		std::string		_authorization; // example: Basic YWxhZGRpbjpvcGVuc2VzYW1l
										// The second is: username:password encoded with base64

		std::string		_contentLanguage; // example: de-DE
										// example: en, de-DE, en-CA

		std::string		_contentLength; // example: 55

		std::string		_contentLocation; // documents/foo.json
										// in PUT and POST requests - ignore it

		std::string		_contentType; // for POST or PUT requests
									// example: text/html; charset=UTF-8; text/plain
									// example: multipart/form-data; boundary=something

		std::string		_date; // example: Wed, 21 Oct 2015 07:28:00 GMT

		std::string		_host; // specifies the host and port number of the server
								// If no port => 80 by default
								// It is MANDATORY header
								// If there is no 'Host' header -> Server returns "400 Bad Request"
								// check it in isValid for HTTPRequest!
								// example: developer.mozilla.org
								// example: www.yandex.ru:80

		std::string		_referer; // address of the page making the request
								// http://developer.mozilla.org/en-US/docs/Web/JavaScript
								// http:://example.com/page?q=123
								// http://example.com/

		std::string		_userAgent; // example: 
		// Firefox:
		// Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0
		// Safari:
		// Mozilla/5.0 (iPhone; CPU iPhone OS 13_5_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.1.1 Mobile/15E148 Safari/604.1
		// Chrome:
		// Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36

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
