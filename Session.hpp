/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:06:28 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/23 16:32:03 by gjessica         ###   ########.fr       */
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

# define BUFFER_SIZE 1024

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

		Location		*_serverLocation;
		int				_remoteAddr;
		std::string		_responseFilePath;

		char			_buf[BUFFER_SIZE + 1];
		int				_bufLeft;
		std::string		_requestStr; // It should be 'HTTPRequest' class that owns that string
		char			_write_buffer[BUFFER_SIZE];
		std::string		_responseStr; // It should be 'HTTPResponse' class that owns that string

		bool			_deleteMe;

		std::string		_login; // In some case we may need it. WWWAuthenticate Response header
		std::string		_password; // If we need them but they are empty -> 401 Unauthorized

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
		bool			isCGI(void) const;
		virtual bool	getDeleteMe(void) const;
		virtual void	remove(void);
		virtual void	handle(void);

		void			fillDefaultResponseFields(void);
		bool			fillErrorResponse(int code);

		void			makeGETResponse(void);
		void			makeHEADResponse(void);
		void			makePOSTResponse(void);
		void			makePUTResponse(void);
		void			makeCGIResponse(void);

		// GETTERS
		int				getRemoteAddr(void) const;
		Location		*getServerLocation(void) const;
};

#endif
