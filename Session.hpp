#ifndef SESSION_HPP
# define SESSION_HPP

# define BUFFER_SIZE 1024000

# define NONE 0
# define DIR_PATH 1
# define FILE_PATH 2

# include "Server.hpp"
# include "CGIRequest.hpp"
# include "CGIResponse.hpp"
# include "ASocketOwner.hpp"
# include "HTTPRequest.hpp"
# include "HTTPResponse.hpp"
# include "Server.hpp"
# include "Config.hpp"
# include "Util.hpp"
# include "CGIRequest.hpp"
# include "CGIResponse.hpp"
# include <vector>
# include <list>
# include <string>
# include <iostream>
# include <sstream>
# include <fcntl.h>
# include <cstring>
# include <algorithm>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <dirent.h>
# include <stdlib.h>
# include <fstream>
# include <cstring>
# include <errno.h>

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

		std::string		_requestStr;
		std::string		_responseStr;

		bool			_deleteMe;

		std::string		_login;
		std::string		_password;

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

		Session(void);
		Session(Session const &src);
		Session			&operator=(Session const &rhs);

		std::string		getDirListing(std::string const &path);
		void			setRequestCgiPathTranslated(void) const;

	public:

		Session(int sockfd, int remoteAddr, Server *master);
		virtual ~Session(void);

		void				generateResponse(void);
		void				responseToString(void);
		bool				isValidRequest(void);
		bool				isValidRequestTarget(void);
		bool				isValidRequestAllow(void) const;
		bool				isValidRequestHost(void) const;
		bool				isValidPermissions(void) const;
		bool				isValidBodySize(void) const;
		bool				isCGI(void) const;
		virtual bool		isDeleteMe(void) const;
		virtual void		remove(void);
		virtual void		handle(int action);

		void				fillDefaultResponseFields(void);
		bool				makeErrorResponse(int code);

		void				makeGETResponse(void);
		void				makeHEADResponse(void);
		void				makePOSTResponse(void);
		void				makePUTResponse(void);
		void				makeCGIResponse(void);
		void				makeRedirectionResponse(std::string const &path, \
								int statusCode, std::string const &statusText);
		bool 				isEndRequest(std::string const &_requestStr);
		bool				isHeadersEnd(std::string const &_requestStr);

		 char				**createArgv(void);
		 char				**createEnvp(CGIRequest *cgiRequest);
		 void 				clean(void);

		// GETTERS
		int					getRemoteAddr(void) const;
		Location			*getServerLocation(void) const;
		std::string const	&getResponseFilePath(void) const;
};

#endif
