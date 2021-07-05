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
		bool			_valid;
		std::string		_error;

		// START LINE
		Options			_method;
		std::string		_target;
		std::string		_queryString;
		std::string		_cgiPathInfo;
		std::string		_cgiPathTranslated;
		std::string		_filename;
		std::string		_protocolVersion;

		// HEADERS
		std::string		_acceptCharset;
		std::string		_acceptLanguage;
		char			_allow;
		std::string		_authorization; // example: Basic YWxhZGRpbjpvcGVuc2VzYW1l
										// The second is: username:password encoded with base64
		std::string		_contentLanguage;
		int 			_contentLength;
		std::string		_contentLocation;
		std::string		_contentType;
		std::string		_date; // Wed, 21 Oct 2015 07:28:00 GMT
		std::string		_host; // specifies the host and port number of the server
								// If no port => 80 by default
								// It is MANDATORY header
								// If there is no 'Host' header -> Server returns "400 Bad Request"
								// example: developer.mozilla.org ; www.yandex.ru:80
		std::string		_referer;
		std::string		_userAgent;
		std::string		_transferEncoding;
		std::map<std::string, std::string>	_specialHeaders;

		// BODY
		std::string		_body;

		HTTPRequest(void);
		HTTPRequest(HTTPRequest const &src);
		HTTPRequest		&operator=(HTTPRequest const &rhs);

	public:

		HTTPRequest(std::string &str, Session *session);
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
		std::map<std::string, std::string> &getSpecialHeaders(void);

		// SETTERS
		int					setError(std::string const &str);
		void				setBody(std::string const &str);
		void				setCgiPathTranslated(void);
		void				setCgiPathTranslated(std::string const &str);
		void				setCgiPathInfo(std::string const &str);
		void				setContentLength(int contentLength);
		void				setTarget(std::string const &target);

	private:

		bool 				setStartLineParam(std::string &line);
		void				splitTargetAndCgiPathInfo(void);
		void				parseRequest(std::string &str);
};

#endif
