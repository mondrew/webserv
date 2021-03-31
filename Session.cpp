/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/31 08:49:28 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "Util.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fcntl.h>
#include <cstring>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

Session::Session(int a_sockfd, Server *master) :	ASocketOwner(a_sockfd),
													_theMaster(master),
													_request(0),
													_response(0),
													_serverLocation(0),
													_deleteMe(false) {
	return ;
}

Session::~Session(void) {

	if (_request)
		delete _request;
	if (_response)
		delete _response;
	return ;
}

bool		Session::getDeleteMe(void) const {

	return (this->_deleteMe);
}

bool		Session::isValidRequestTarget(void) {

	// Need to check here if all paths are CORRECT (maybe better to use absotute paths??)
	bool			isValidPath = false;
	bool			isValidFolder = false;
	std::string		pathLoc;
	int				pathType = DIR_PATH;

	std::string		reqPath;
	// Now check - is there such folder or not
	for (std::vector<Location *>::iterator it = \
								this->_theMaster->getLocationSet().begin();
									it < this->_theMaster->getLocationSet().end(); it++)
	{
		reqPath = (*it)->getRoot() + this->_request->getTarget();
		// If it is not folder -> delete last part of the path (it must be filename)
		if (!Util::isDirectory(reqPath))
		{
			pathType = FILE_PATH;
			reqPath = Util::removeLastPath(reqPath);
			if (reqPath.empty())
				return (false);
		}
		pathLoc = (*it)->getRoot() + (*it)->getLocationPath();
		if (pathLoc.compare(reqPath) == 0)
		{
			// Here we check the Path to the concrete file
			isValidFolder = true;
			this->_serverLocation = *it;
			if (pathType == DIR_PATH)
			{
				std::string		delim = "/";
				if (Util::getLastChar(reqPath) == '/')
					delim = "";
				this->_responseFilePath = reqPath + delim + (*it)->getIndex();
				isValidPath = true;
			}
			break ;
		}
	}

	if (isValidFolder && (pathType == FILE_PATH))
	{
		if (Util::isCGI(this->_request->getTarget()))
		{
			// First lets check www.localhost:8002/cgi-bin/script.cgi
			reqPath = this->_serverLocation->getRoot() + this->_request->getTarget();
			if (!Util::exists(reqPath))
			{
				// Then lets check www.localhost:8002/script.cgi
				reqPath = this->_serverLocation->getCgiPath() + this->_request->getTarget();
				if (!Util::exists(reqPath))
					return (false);
			}
		}
		else
		{
			reqPath = this->_serverLocation->getRoot() + this->_request->getTarget();
			if (!Util::exists(reqPath))
				return (false);
		}
		isValidPath = true;
		this->_responseFilePath = reqPath;
	}
	// std::cout << "|||||||||| is valid req target: " << isValidPath << std::endl; // debug
	// std::cout << "|||||||||| is valid folder: " << isValidFolder << std::endl; // debug
	// std::cout << "|||||||||| responseFilePath: " << _responseFilePath << std::endl; // debug
	return (isValidPath);
}

bool		Session::isValidRequestAllow(void) const {

	return ((this->_serverLocation->getLimitExcept() == 0) || \
			(_request->getMethod() & this->_serverLocation->getLimitExcept()));
}

bool		Session::isValidRequestHost(void) const {

	return (!_request->getHost().empty());
}

bool		Session::isValidPermissions(void) const {

	return (Util::isAllowedToRead(this->_responseFilePath));
}

bool		Session::isCGI(void) const {

	return (Util::isCGI(this->_responseFilePath));
}

void		Session::generateResponse(void) {

	this->_request = new HTTPRequest(_requestStr);
	this->_response = new HTTPResponse();

	// Set common fields (independent of response type)
	_response->setProtocolVersion("HTTP/1.1");
	_response->setDate(Util::getDate());
	_response->setServer("MGINX Webserv/1.0 (Unix)");
	_response->setContentLanguage("en, en-US, ru");

	_response->setAllow(0);
	_response->setLastModified("");
	_response->setLocation("");
	_response->setRetryAfter("");
	_response->setTransferEncoding("");
	_response->setWWWAuthenticate("");
	_response->setContentLocation("");
	_response->setContentType("text/html");

	if (!_request->isValid() || !this->isValidRequestHost())
	{
		_response->setStatusCode(400);
		_response->setStatusText("Bad Request");

		_responseFilePath = (_theMaster->getErrorMap())[400];
		_response->setBody(Util::fileToString((this->_responseFilePath)));
		_response->setContentLength(_response->getBody().length());
	}
	else if (!isValidRequestTarget())
	{
		_response->setStatusCode(404);
		_response->setStatusText("Not Found");

		_responseFilePath = _theMaster->getErrorMap()[404];
		_response->setBody(Util::fileToString((this->_responseFilePath)));
		_response->setContentLength(_response->getBody().length());
	}
	else if (!isValidRequestAllow())
	{
		_response->setStatusCode(405);
		_response->setStatusText("Method Not Allowed");

		_responseFilePath = _theMaster->getErrorMap()[405];
		_response->setBody(Util::fileToString((this->_responseFilePath)));
		_response->setContentLength(_response->getBody().length());
	}
	else if (!isValidPermissions())
	{
		_response->setStatusCode(403);
		_response->setStatusText("Forbidden");

		_responseFilePath = _theMaster->getErrorMap()[403];
		_response->setBody(Util::fileToString((this->_responseFilePath)));
		_response->setContentLength(_response->getBody().length());
	}
	else
	{
		//std::cout << "START - Parser testing - HTTPRequest\n";
		//_request->print();
		//std::cout << "END - Parser testing - HTTPRequest\n\n";

		if (_request->getMethod() == GET)
		{
			// return file or run script
			_response->setStatusCode(200);
			_response->setStatusText("OK");
			_response->setTransferEncoding("identity");

			_response->setAllow(_serverLocation->getLimitExcept());
			_response->setLocation(this->_responseFilePath);
			if (isCGI())
			{
				pid_t				pid;
				int					pipefd[2];
				std::ostringstream	oss;

				/*
				* SET ENVIRONMENT VARIABLES FOR CGI
				*/

				if (pipe(pipefd) == -1)
				{
					std::cout << "Pipe failed" << std::endl;
					// Internal Server Error
				}
				if ((pid = fork()) == -1)
				{
					std::cout << "Fork failed" << std::endl;
					// Internal Server Error
				}
				if (pid == 0)
				{
					// Child
					// Close IN pipe side
					close(pipefd[0]);

					// Replace stdout with WRITE-END of the PIPE
					dup2(STDOUT_FILENO, pipefd[1]);

					// Run the CGI script
					char	*s1 = NULL;
					char	*s2 = NULL;
					char *const	argv[2] = {s1, s2};
					char *const	envp[2] = {s1, s2};
					execve(_responseFilePath.c_str(), argv, envp);

					exit(0);
				}
				// Parent
				// Close OUT pipe side
				close(pipefd[1]);

				// Replace stdin with READ-END of the PIPE
				dup2(STDIN_FILENO, pipefd[0]);

				oss << std::cin;
				oss << std::endl;

				_response->setBody(oss.str());
				_response->setContentLength(_response->getBody().length());

				_response->setRetryAfter("");
				_response->setWWWAuthenticate("");
				_response->setContentLocation(this->_responseFilePath);

				// Change it: get info from CGI
				_response->setContentType(\
								Util::detectContentType(this->_responseFilePath));
				_response->setLastModified(\
							Util::getFileLastModified(this->_responseFilePath));

				// Wait for the child
				waitpid(pid, 0, 0);
			}
			else

			{
				_response->setBody(Util::fileToString((this->_responseFilePath)));
				_response->setContentLength(_response->getBody().length());

				_response->setRetryAfter("");
				_response->setWWWAuthenticate("");
				_response->setContentLocation(this->_responseFilePath);
				_response->setContentType(\
								Util::detectContentType(this->_responseFilePath));
				_response->setLastModified(\
							Util::getFileLastModified(this->_responseFilePath));
				}
		}
		else if (_request->getMethod() == POST)
		{
			// update some info?
			_response->setStatusCode(200);
			_response->setStatusText("OK");
		}
		else if (_request->getMethod() == PUT)
		{
			// Save file to ./www/upload
			_response->setStatusCode(201);
			_response->setStatusText("Created");
			// OR
			// _response->setStatusCode(204);
			// _response->setStatusText("No Content");
		}
		else if (_request->getMethod() == HEAD)
		{
			// do smth
			_response->setStatusCode(200);
			_response->setStatusText("OK");
		}
	}
	responseToString();
}

void		Session::responseToString(void) {

	std::ostringstream	oss;

	// Status Line
	oss << _response->getProtocolVersion() << " " << _response->getStatusCode();
	oss << " " << _response->getStatusText() << std::endl;
	// Headers
	oss << "Allow: " << Util::allowToString(_response->getAllow()) << std::endl;
	oss << "Content-Language: " << _response->getContentLanguage() << std::endl;
	oss << "Content-Length: " << _response->getContentLength() << std::endl;
	oss << "Content-Type: " << _response->getContentType() << std::endl;
	oss << "Date: " << _response->getDate() << std::endl;
	oss << "Last-Modified: " << _response->getLastModified() << std::endl;
	oss << "Location: " << _response->getLocation() << std::endl;
	oss << "Retry-After: " << _response->getRetryAfter() << std::endl;
	oss << "Server: " << _response->getServer() << std::endl;
	oss << "Transfer-Encoding: " << _response->getTransferEncoding() << std::endl;
	oss << "WWW-Authenticate: " << _response->getWWWAuthenticate() << std::endl;
	oss << std::endl;
	// Body
	oss << _response->getBody() << std::endl; // Is it nessecary to add "\n" after last line?

	_responseStr = oss.str();
	// std::cout << "============Response START==============================" << std::endl;
	// std::cout << _responseStr << std::endl;
	// std::cout << "============Repsonse END==============================" << std::endl;
}

void		Session::remove(void) {

	_theMaster->removeSession(this);
}

// MONDREW handle
void		Session::handle(void) {

	int		ret;

	if (getWantToRead() == true)
	{
		ret = read(_socket, _buf, BUFFER_SIZE);
		if (ret < 0)
		{
			// Exceptions will be better!
		}
		else if (ret == 0 || ret < BUFFER_SIZE)
		{
			// EOF reached
			_buf[ret] = '\0';
			_requestStr += _buf;
			std::cout << _requestStr << std::endl; // debug
			generateResponse();
			setWantToRead(false);
			setWantToWrite(true);
		}
		else
		{
			_buf[ret] = '\0';
			_requestStr += _buf;
		}
	}
	else if (getWantToWrite() == true)
	{
		if (!_responseStr.empty())
		{
			ret = write(_socket, _responseStr.c_str(), \
				std::min(BUFFER_SIZE, static_cast<int>(_responseStr.length())));
			_responseStr.erase(0, ret);
		}
		else
		{
			// We have sent all HTTP Response
			// Now we have to close the Session
			// HTTP is connectionless protocol !
			_deleteMe = true;
		}
	}
}
