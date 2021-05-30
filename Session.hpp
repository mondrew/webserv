/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:06:28 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/30 17:42:01 by gjessica         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_HPP
# define SESSION_HPP

#include "Server.hpp"
#include "CGIRequest.hpp"
#include "CGIResponse.hpp"
#include "ASocketOwner.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

# define BUFFER_SIZE 102400

# define NONE 0
# define DIR_PATH 1
# define FILE_PATH 2

//# define NONE 0
//# define FOLDER_PATH 1
//# define FILE_PATH 2

class Server;
class HTTPRequest;
class CGIRequest;
class CGIResponse;

class Session : public ASocketOwner {

	private:

		Server			*_theMaster;
		HTTPRequest		*_request;
		HTTPResponse	*_response;

		CGIRequest		*_cgiRequest;
		CGIResponse		*_cgiResponse;

		Location		*_serverLocation;
		int				_remoteAddr;
		std::string		_responseFilePath;
		std::string		_responseFilePathOld;

		std::string		_requestStr; // It should be 'HTTPRequest' class that owns that string
		std::string		_responseStr; // It should be 'HTTPResponse' class that owns that string

		bool			_deleteMe;

		std::string		_login; // In some case we may need it. WWWAuthenticate Response header
		std::string		_password; // If we need them but they are empty -> 401 Unauthorized

		bool			_validRequestFlag;
		std::string		_readStr;

		pid_t			_pid;
		int				_defStdIn;
		int				_defStdOut;
		FILE			*_fileCGIRequest;
		FILE			*_fileCGIResponse;
		off_t			_offset;
		bool			_launchChild;

		std::string		_msgForCGI;

		int				_contentLength;
		int				_headersLength;

		//char			_buf[BUFFER_SIZE + 1];
		//int				_bufLeft;
		//char			_write_buffer[BUFFER_SIZE];

		Session(void);
		Session(Session const &src);
		Session			&operator=(Session const &rhs);

		std::string		getDirListing(std::string const &path);
		void			setRequestCgiPathTranslated(void) const;

	public:

		Session(int sockfd, int remoteAddr, Server *master);
		virtual ~Session(void);

		void			generateResponse(void);
		void			responseToString(void);
		bool			isValidRequest(void);
		bool			isValidRequestTarget(void);
		bool			isValidRequestAllow(void) const;
		bool			isValidRequestHost(void) const;
		bool			isValidPermissions(void) const;
		bool			isValidBodySize(void) const;
		bool			isCGI(void) const;
		virtual bool	isDeleteMe(void) const;
		virtual void	remove(void);
		virtual void	handle(int action);

		void			fillDefaultResponseFields(void);
		bool			makeErrorResponse(int code);

		void			makeGETResponse(void);
		void			makeHEADResponse(void);
		void			makePOSTResponse(void);
		void			makePUTResponse(void);
		void			makeCGIResponse(void);
		void			makeRedirectionResponse(std::string const &path, \
								int statusCode, std::string const &statusText);
		// void			checkNeedToRead(void);

		bool 			isEndRequest(std::string const &_requestStr);
		bool			isHeadersEnd(std::string const &_requestStr);


		 char		**createArgv(void);
		 char		**createEnvp(CGIRequest *cgiRequest);
		 void 		clean(void);

		// GETTERS
		int					getRemoteAddr(void) const;
		Location			*getServerLocation(void) const;
		std::string const	&getResponseFilePath(void) const;

		// SETTERS
};

#endif
