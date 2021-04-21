/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/21 12:46:00 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "Util.hpp"
#include "CGIRequest.hpp"
#include "CGIResponse.hpp"
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
#include <dirent.h>
#include <stdlib.h>

// Query string added

Session::Session(int a_sockfd, int remoteAddr, Server *master) :
													ASocketOwner(a_sockfd),
													_theMaster(master),
													_request(0),
													_response(0),
													_serverLocation(0),
													_remoteAddr(remoteAddr),
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

	// Need to check here if all paths are CORRECT
	// (maybe better to use absotute paths??)
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
		// If it is not folder -> delete last part of the path (filename)
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
		if (Util::isCGI(this->_request->getTarget()) && \
										reqPath.compare((*it)->getRoot()) != 0)
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
				if (this->_serverLocation->isAutoindex() &&
						((*it)->getIndex().empty() || \
					 		!Util::exists(reqPath + delim + (*it)->getIndex())))
				{
					this->_responseFilePath = reqPath + delim;
				}
				else
					this->_responseFilePath = reqPath + delim + \
										  					(*it)->getIndex();
				// Logger::msg("RFP = " +this->_responseFilePath); // debug

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
			reqPath = this->_serverLocation->getRoot() + \
					  								this->_request->getTarget();
			if (!Util::exists(reqPath))
			{
				// Then lets check www.localhost:8002/script.cgi
				reqPath = this->_serverLocation->getCgiPath() + \
				  									this->_request->getTarget();
				if (!Util::exists(reqPath))
					return (false);
			}
		}
		else
		{
			reqPath = this->_serverLocation->getRoot() + \
					  								this->_request->getTarget();
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

	CGIRequest 			cgiRequest(this->_request);
	pid_t				pid;
	int					pipefd[2];
	std::ostringstream	oss;

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

		std::cout << "Hello from child!" << std::endl; // debug
		// Replace stdout with WRITE-END of the PIPE
		dup2(STDOUT_FILENO, pipefd[1]);
		// dup2(pipefd[1], STDOUT_FILENO); // testingggggggggggggggggggggggggg

		// Run the CGI script

		if ((this->_request->getMethod() || this->_request->getMethod()) && \
			(this->_request->getQueryString().find("=")) != std::string::npos)
		{
		}
		const char	**argv = static_cast<const char **>(malloc(sizeof(char *) * 2));
		argv[0] = this->_request->getTarget().c_str();
		argv[1] = 0;
		//const char *argv[2] = {this->_request->getTarget().c_str()}; // добавить в CGIRequest
		const char	**envp = static_cast<const char **>(malloc(sizeof(char *) * 17));
		envp[0] = cgiRequest.getAuthType().c_str();
		envp[1] = cgiRequest.getContentLength().c_str();
		envp[2] = cgiRequest.getGatewayInterface().c_str();
		envp[3] = cgiRequest.getPathInfo().c_str();
		envp[4] = cgiRequest.getPathTranslated().c_str();
		envp[5] = cgiRequest.getQueryString().c_str();
		envp[6] = cgiRequest.getRemoteAddr().c_str();
		envp[7] = cgiRequest.getRemoteIdent().c_str();
		envp[8] = cgiRequest.getRemoteUser().c_str();
		envp[9] = cgiRequest.getRequestMethod().c_str();
		envp[10] = cgiRequest.getRequestURI().c_str();
		envp[11] = cgiRequest.getScriptName().c_str();
		envp[12] = cgiRequest.getServerName().c_str();
		envp[13] = cgiRequest.getServerPort().c_str();
		envp[14] = cgiRequest.getServerProtocol().c_str();
		envp[15] = cgiRequest.getServerSoftware().c_str();
		envp[16] = 0;

		// 1. [ GET ] method - produce the document based on: meta-variables
		// 		- should parse the query string to the array on words (argv)
		//		- if the query String doesn't contain unencoded '=' character, we should 
		//			interpret query string as 'indexed' HTTP query:
		//			a. Parse it (delimiter if '+')
		//			b. Decode each string
		//		- decode queryString Util::decodeUriEncoded(
		//
		// 2. [ HEAD ] method - needs from script only headers, not body
		// 		- if script returns the body - then the server MUST discard it
		//		- same as in [ GET ]
		//
		// 3. [ POST ] method - produce the document based on: 
		// 		meta-variables & data in request message-body
		// 		it MUST check CONTENT_LENGTH and CONTENT_TYPE
		// 		- check Content-Type header: if 'application/x-www-form-urlencoded' -> decode body
		execve(_responseFilePath.c_str(), \
			const_cast<char *const*>(argv), const_cast<char * const*>(envp));

		exit(0);
	}
	// Parent
	// Close OUT pipe side
	close(pipefd[1]);

	// Replace stdin with READ-END of the PIPE
	dup2(STDIN_FILENO, pipefd[0]);

	oss << std::cin;
	oss << std::endl;

	std::cout << "OSS===++++>>>: " << oss.str() << std::endl; // endl

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

std::string		Session::getDirListing(std::string const &path)
{
	DIR					*dir;
	struct dirent		*diread;
	std::ostringstream	str;

	//path ./www/pages/test/
	// std::cout << "!!!!!!!!path " << path << std::endl; // debug
	str << "<html><head><title>MGINX</title></head>";
	str << "<body>";
	if ((dir = opendir((  path + "/").c_str())) != 0)
	{
		// Fix it - so the first will be '.' then '..' and then other directories
        while ((diread = readdir(dir)) != 0) {
			// std::cout << "!!!diread->d_name " << diread->d_name << std::endl; // debug
			// std::cout << "!!!this->_request->getTarget() " << this->_request->getTarget() << "\n";
			str << "<a href='" << this->_request->getTarget();
			if (this->_request->getTarget().compare("/") != 0)
				str << "/";
			str << (diread->d_name) << "'>";
			str << (diread->d_name);
			str <<  "</a>";
			str << "<br />";
        }
        closedir (dir);
    }
	else
	{
		// Internal Server error
		Logger::e("Error open directory " + path);
        return "";
    }
	return (str.str());
}

void		Session::makeGETResponse(void) {

	// return file or run script
	// std::cout << "cgi" << std::endl; // debug
	_response->setStatusCode(200);
	_response->setStatusText("OK");
	_response->setTransferEncoding("identity");

	_response->setAllow(_serverLocation->getLimitExcept());
	_response->setLocation(this->_responseFilePath);

	if (isCGI())
	{
		if (Util::printRequestType)
			std::cout << "REQUEST_TYPE: ==>==> CGI <==<==" << std::endl;
		makeCGIResponse();
	}
	else
	{
		if (Util::printRequestType)
			std::cout << "REQUEST_TYPE: ==>==> GET <==<==" << std::endl;
		//NEW BLOCK
		// Logger::msg("ResponsePath - " + this->_responseFilePath); // debug
		if (!Util::isDirectory(this->_responseFilePath))
			_response->setBody(Util::fileToString((this->_responseFilePath)));
		else
			_response->setBody(getDirListing(this->_responseFilePath));
		//END NEW BLOCK
		//_response->setBody(Util::fileToString((this->_responseFilePath)));
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

	this->_request = new HTTPRequest(_requestStr, this);
	this->_response = new HTTPResponse();

	// Set common fields (independent of response type)
	fillDefaultResponseFields();
	if (isValidRequest())
	{
		setRequestCgiPathTranslated();
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
	// if (this->_request->getMethod() != GET && this->_request->getMethod() != HEAD)
	oss << _response->getBody() << std::endl; // Is it nessecary to add "\n" after last line?

	_responseStr = oss.str();

	if (Util::printResponses)
	{
		std::cout << "=====================Response START==================" << std::endl;
		std::cout << _responseStr;
		std::cout << "=====================Repsonse END====================" << std::endl;
	}
}

void		Session::setRequestCgiPathTranslated(void) const {

	if (!this->_request->getCgiPathInfo().empty())
		this->_request->setCgiPathTranslated();
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
				std::cout << _requestStr << std::endl;
				// _request->print(); // wrong! we don't have instance of HTTPRequest class yet!!!
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
	// Logger::msg("Target - " + _request->getTarget()); // debug
}

int			Session::getRemoteAddr(void) const {

	return (this->_remoteAddr);
}

Location	*Session::getServerLocation(void) const {

	return (this->_serverLocation);
}
