/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 15:48:48 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/30 21:46:05 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP
# include <iostream>
# include <string>
# include <sstream>
# include "Util.hpp"
# include "Session.hpp"

class Session;

class HTTPRequest {

	private:

		Session			*_session;
		// https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages

		bool			_valid; // = true by default
		std::string		_error;
		// START LINE
		Options			_method; // GET, PUT, POST, HEAD, OPTIONS, CONNECT
								// Question: is there can be several methods??? Do list?
		std::string		_target; // URI
		std::string		_queryString;
		std::string		_cgiPathInfo;
		std::string		_cgiPathTranslated;
		std::string		_filename;

		std::string		_protocolVersion;

		// HEADERS
		// USE THIS vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
		// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers
		// USE THIS ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
		// Subject
		std::string		_acceptCharset; // example: utf-8, iso-8859-15;q=0.8 // can be empty
										// SERVER RESPONDS with 'Content-Type' or ignore

		std::string		_acceptLanguage; // example: en, en-US, fr-CA, fr;q=0.9
										// SERVER RESPONDS with 'Content-Language' or ignore

		char			_allow; // example: GET, POST, PUT, HEAD
								// can be empty (if there are no allowed methods)

		std::string		_authorization; // example: Basic YWxhZGRpbjpvcGVuc2VzYW1l
										// The second is: username:password encoded with base64

		std::string		_contentLanguage; // example: de-DE
										// example: en, de-DE, en-CA

		int 			_contentLength; // example: 55

		std::string		_contentLocation; // documents/foo.json
										// in PUT and POST requests - ignore it

		std::string		_contentType; // for POST or PUT requests
									// example: application/js, text/html; charset=UTF-8, text/plain
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

		std::string		_transferEncoding; // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Transfer-Encoding
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

		HTTPRequest(std::string const &str, Session *session);
		~HTTPRequest(void);

		bool				isValid(void) const;
		void				print(void) const;

		// GETTERS
		int					getContentLength(void) const;
		char 				getAllow(void) const;
		Options				getMethod(void) const;
		std::string			getMethodName(void) const;
		std::string const	&getTarget(void) const;
		std::string const	&getQueryString(void) const;
		std::string const	&getProtocolVersion(void) const;
		std::string const	&getAcceptCharset(void) const;
		std::string const	&getAcceptLanguage(void) const;
		std::string const	&getAuthorization(void) const;
		std::string 		getAuthorizationType(void) const;
		std::string 		getAuthorizationData(void) const;
		std::string const	&getContentLanguage(void) const;
		std::string const	&getContentLocation(void) const;
		std::string const	&getContentType(void) const;
		std::string const	&getDate(void) const;
		std::string const	&getHost(void) const;
		std::string const	&getReferer(void) const;
		std::string const	&getUserAgent(void) const;
		std::string const	&getBody(void) const;
		std::string const	&getError(void) const;
		std::string const	&getCgiPathInfo(void) const;
		std::string const	&getCgiPathTranslated(void) const;
		std::string const	&getTransferEncoding(void) const;

		int					getBodyLength(void) const;

		Session				*getSession(void) const;

		std::string const	&getFilename(void) const;

		// SETTERS
		int					setError(std::string const &str);
		void				setBody(std::string const &str);
		void				setCgiPathTranslated(void);

	private:

		bool 				setStartLineParam(std::string &line);
		void				splitTargetAndCgiPathInfo(void);
		void				parseRequest(std::string const &str);
};

#endif
