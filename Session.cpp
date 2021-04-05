/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/05 11:05:57 by mondrew          ###   ########.fr       */
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
		// std::cout << "===> reqPath: " << reqPath << std::endl; // debug
		// If it is not folder -> delete last part of the path (it must be filename)
		if (!Util::isDirectory(reqPath))
		{
			pathType = FILE_PATH;
			reqPath = Util::removeLastPath(reqPath);
			// std::cout << "===> reqPath (removed file): " << reqPath << std::endl; // debug
			if (reqPath.empty())
				return (false);
		}
		if (Util::getLastChar(reqPath) == '/')
			reqPath = Util::removeLastPath(reqPath); // for removing '/' at the end
		if (Util::isCGI(this->_request->getTarget()) && reqPath.compare((*it)->getRoot()) != 0)
				pathLoc = (*it)->getCgiPath();
		else
			pathLoc = (*it)->getRoot() + (*it)->getLocationPath();
		if (Util::getLastChar(pathLoc) == '/')
			pathLoc = Util::removeLastPath(pathLoc); // for removing '/' at the end
		// std::cout << "===> pathLoc: " << pathLoc << std::endl; // debug
		// std::cout << "------------------------------" << std::endl; // debug
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
	if (Util::printRequestTarget)
	{
		if (isValidFolder)
			std::cout << "Folder Path is valid" << std::endl;
		else
			std::cout << "Folder Path is invalid" << std::endl;
		if (isValidPath)
			std::cout << "File Path is valid" << std::endl;
		else
			std::cout << "File Path is invalid" << std::endl;
		std::cout <<  "RESPONSE FIlE PATH: " << _responseFilePath << std::endl;
	}
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

void		Session::fillDefaultResponseFields(void) {

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
}

bool		Session::fillErrorResponse(int code) {

	_response->setStatusCode(code);
	if (code == 400)
		_response->setStatusText("Bad Request");
	else if (code == 401)
		_response->setStatusText("Unauthorized");
	else if (code == 403)
		_response->setStatusText("Forbidden");
	else if (code == 404)
		_response->setStatusText("Not Found");
	else if (code == 405)
		_response->setStatusText("Method Not Allowed");
	else if (code == 408)
		_response->setStatusText("Request Timeout");
	else if (code == 500)
		_response->setStatusText("Internal Server Error");
	else if (code == 501)
		_response->setStatusText("Not Implemented");
	else if (code == 503)
		_response->setStatusText("Service Unavailable");
	else if (code == 505)
		_response->setStatusText("HTTP Version Not Supported");
	_responseFilePath = (_theMaster->getErrorMap())[code];
	_response->setBody(Util::fileToString((this->_responseFilePath)));
	_response->setContentLength(_response->getBody().length());
	return (false);
}

bool		Session::isValidRequest(void) {

	//if (!_request->isValid() && _request->getMethod() == UNKNOWN)
	if (_request->getMethod() == UNKNOWN)
		return (fillErrorResponse(501));
	else if (!_request->isValid() || !this->isValidRequestHost())
		return (fillErrorResponse(400));
	else if (!isValidRequestTarget())
		return (fillErrorResponse(404));
	else if (!isValidRequestAllow())
		return (fillErrorResponse(405));
	else if (!isValidPermissions())
		return (fillErrorResponse(403));
	return (true);
}

void			Session::makeCGIResponse(void) {

	pid_t				pid;
	int					pipefd[2];
	std::ostringstream	oss;

	/*
	* SET ENVIRONMENT VARIABLES FOR CGI
	*/
	//Util

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
		std::cout << "WHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT?" << std::endl;
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

void		Session::makeGETResponse(void) {

	// return file or run script
	_response->setStatusCode(200);
	_response->setStatusText("OK");
	_response->setTransferEncoding("identity");

	_response->setAllow(_serverLocation->getLimitExcept());
	_response->setLocation(this->_responseFilePath);
	if (isCGI())
		makeCGIResponse();
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

void		Session::makeHEADResponse(void) {

	// do smth
	_response->setStatusCode(200);
	_response->setStatusText("OK");
}

void		Session::makePOSTResponse(void) {
	// do smth
	// update some info?
	_response->setStatusCode(201);
	_response->setStatusText("OK");
}

void		Session::makePUTResponse(void) {

	if (_request->getContentLength() == 0 && !_request->getBody().empty())
		_request->setBody(Util::unchunkData(_request->getBody()));

	// Save file to ./www/upload
	_response->setStatusCode(201);
	_response->setStatusText("Created");
	// OR
	// _response->setStatusCode(204);
	// _response->setStatusText("No Content");
}

void		Session::generateResponse(void) {

	this->_request = new HTTPRequest(_requestStr);
	this->_response = new HTTPResponse();

	// Set common fields (independent of response type)
	fillDefaultResponseFields();
	if (isValidRequest())
	{
		if (_request->getMethod() == GET)
			makeGETResponse();
		else if (_request->getMethod() == HEAD)
			makeHEADResponse();
		else if (_request->getMethod() == POST)
			makePOSTResponse();
		else if (_request->getMethod() == PUT)
			makePUTResponse();
	}
	responseToString();
}

void		Session::responseToString(void) {

	std::ostringstream	oss;

	// Status Line
	oss << _response->getProtocolVersion() << " " << _response->getStatusCode();
	oss << " " << _response->getStatusText() << std::endl;
	// Headers
	oss << "Server: " << _response->getServer() << std::endl;
	oss << "Date: " << _response->getDate() << std::endl;
	oss << "Content-Type: " << _response->getContentType() << std::endl;
	oss << "Content-Length: " << _response->getContentLength() << std::endl;
	oss << "Content-Language: " << _response->getContentLanguage() << std::endl;
	oss << "Last-Modified: " << _response->getLastModified() << std::endl;
	oss << "Allow: " << Util::allowToString(_response->getAllow()) << std::endl;
	oss << "Location: " << _response->getLocation() << std::endl;
	oss << "Retry-After: " << _response->getRetryAfter() << std::endl;
	oss << "Transfer-Encoding: " << _response->getTransferEncoding() << std::endl;
	oss << "WWW-Authenticate: " << _response->getWWWAuthenticate() << std::endl;
	oss << std::endl;
	// Body
	oss << _response->getBody() << std::endl; // Is it nessecary to add "\n" after last line?

	_responseStr = oss.str();

	if (Util::printResponses)
	{
		std::cout << "=====================Response START==================" << std::endl;
		std::cout << _responseStr;
		std::cout << "=====================Repsonse END====================" << std::endl;
	}
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
			if (Util::printRequests)
			{
				std::cout << "================== HTTPRequest - START ===================\n";
				_request->print();
				std::cout << "================== HTTPRequest - END ===================\n";
			}
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
