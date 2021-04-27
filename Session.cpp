/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/27 13:17:00 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "Util.hpp"
#include "CGIRequest.hpp"
#include "CGIResponse.hpp"
#include <vector>
#include <list>
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
#include <fstream>

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

	bool			isValidPath = false;
	bool			isValidFolder = false;
	std::string		reqPath = this->_request->getTarget(); // current request path
	std::string		fullPath;
	std::string		cleanPath;

	if (Util::getLastChar(reqPath) == '/')
		reqPath = Util::removeLastPath(reqPath); // for removing '/' at the end

	// Check is there such Location in the config file
	// For CGI -> create special location in config '/cgi-bin'
	for (std::vector<Location *>::iterator it = \
						this->_theMaster->getLocationSet().begin();
							it < this->_theMaster->getLocationSet().end(); it++)
	{
		if ((*it)->isContainedInPath(reqPath))
		{
			cleanPath = reqPath.erase(0, ((*it)->getLocationPath()).size());
			this->_serverLocation = *it;
			if (!cleanPath.empty() && cleanPath[0] != '/')
				cleanPath.insert(0, "/");
			fullPath = (*it)->getRoot() + cleanPath;
			isValidFolder = true;
			break ;
		}
	}
	if (!isValidFolder)
		return (false);

	this->_responseFilePath = fullPath;
	// std::cout << "===>>> FULL PATH: " << fullPath << std::endl; // debug

	if (Util::isDirectory(fullPath))
	{
		std::string		delim = "/";
		if (Util::getLastChar(fullPath) == '/')
			delim = "";

		if (!this->_serverLocation->getIndex().empty() && \
				Util::exists(fullPath + delim + this->_serverLocation->getIndex()))
			this->_responseFilePath += delim + this->_serverLocation->getIndex();
		else
		{
			if (!this->_serverLocation->isAutoindex())
				return (false);
			//this->_responseFilePath += delim; // do we need delim here?
		}
		isValidPath = true;
	}
	else
	{
		if (!Util::exists(fullPath))
			return (false);
		isValidPath = true;
	}

	// Print request target
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
		std::cout <<  "RESPONSE FIlE PATH: " << this->_responseFilePath << std::endl;
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
	if (!(_request->getMethod() & HEAD))
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
	CGIResponse			cgiResponse;
	pid_t				pid;
	int					pipefd[2];
	int					saveStdInFd;
	std::ostringstream	oss;

	/*
	std::cout << "===---Print parsed HTTPRequest instance---===" << std::endl; // debug
	this->_request->print();
	std::cout << "===---END Print parsed HTTPRequest instance---===" << std::endl; // debug
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

		// Make STDOUT be the copy of the pipefd[1] (out pipe end)
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		// dup2(pipefd[1], STDOUT_FILENO);

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

	// Save stdin fd
	saveStdInFd = dup(STDIN_FILENO);

	// Replace stdin with READ-END of the PIPE
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	// dup2(pipefd[0], saveStdInFd);
	// dup2(STDIN_FILENO, pipefd[0]);


	/*
	char	buf[1024];
	int		ret;

	while ((ret = read(0, buf, 1023)))
	{
		buf[ret] = '\0';
		oss << buf;
	}
	*/

	oss << std::cin.rdbuf();
	oss << std::endl;

	// std::cout << "OSS===++++>>>: " << oss.str() << std::endl; // endl
	if (Util::printCGIResponseString) {
		std::cout << "-----= [ Pure CGI Response String From Child ] =-----" << std::endl;
		std::cout << "[CGI_STRING]: " << oss.str();
		std::cout << "-----= [ END CGI Response String ] =-----" << std::endl;
		std::cout << std::endl;
	}

	cgiResponse.parseCGIResponse(oss.str());
	_response->setBody(cgiResponse.getBody());
	_response->setContentType(cgiResponse.getContentType());
	_response->setContentLength(_response->getBody().length());

	_response->setRetryAfter("");
	_response->setWWWAuthenticate("");
	_response->setContentLocation(this->_responseFilePath);

	// Change it: get info from CGI
	_response->setLastModified(\
				Util::getFileLastModified(this->_responseFilePath));

	// Wait for the child
	waitpid(pid, 0, 0);
}

std::string		Session::getDirListing(std::string const &path)
{
	DIR						*dir;
	struct dirent			*diread;
	std::ostringstream		oss;
	std::list<std::string>	files;

	oss << "<html><head><title>MGINX</title></head>";
	oss << "<body>";
	if ((dir = opendir((path).c_str())) != 0)
	{
		// Fix it - so the first will be '.' then '..' and then other directories
        while ((diread = readdir(dir)) != 0)
		{
			if (std::string(diread->d_name).compare(".") == 0)
				files.push_front(std::string(diread->d_name));
			else if (std::string(diread->d_name).compare("..") == 0)
			{
				if (files.size() && (files.begin())->compare(".") == 0)
					files.insert(++files.begin(), std::string(diread->d_name));
				else
					files.push_front(std::string(diread->d_name));
			}
			else
				files.push_back(std::string(diread->d_name));
		}
		for (std::list<std::string>::iterator it = files.begin();\
														it != files.end(); it++)
		{
			oss << "<a href='" << this->_request->getTarget();
			if (Util::getLastChar(this->_request->getTarget()) != '/')
				oss << "/";
			oss << *it << "'>";
			oss << *it;
			oss <<  "</a>";
			oss << "<br />";
			// std::cout << "oss: " << oss.str() << std::endl; // debug
        }
        closedir (dir);
    }
	else
	{
		// Internal Server error
		Logger::e("Error open directory " + path);
        return "";
    }
	return (oss.str());
}

/*
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
*/

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
		// Autoindex
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

std::string		generateFilename()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	std::stringstream ss;
	ss << ms;
    std::string res  = ss.str();

	return res;
}

std::string		createFileUpload(std::string const &exp, std::string const &body){
	std::string filename;

	filename = generateFilename();

	std::string tmp = "./www/upload/" + filename + "." + exp;
	std::ofstream outfile(tmp.c_str());

	outfile << body;
	outfile.close();
	return filename + "." + exp;
}

void		Session::makePOSTResponse(void) {
	// do smth
	// update some info?
	_response->setStatusCode(200);
	_response->setStatusText("OK");
	_response->setTransferEncoding(_request->getTransferEncoding());
	_response->setAllow(_serverLocation->getLimitExcept());
	_response->setLocation(this->_responseFilePath);
	_response->setBody("");

	if (isCGI())
		makeCGIResponse();
	else
	{
		std::string conType = _request->getContentType();
		if (Util::getTypeByMime(conType) != "unk")
		{
			std::string filename = createFileUpload(Util::getTypeByMime(conType), _request->getBody());
			_response->setStatusCode(201);
			_response->setStatusText("Created");
			_response->setLocation("./www/upload/" + filename);
		}
	}
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
			makePOSTResponse();
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
	oss << "Content-Location: " << _response->getContentLocation() << std::endl;
	oss << "Last-Modified: " << _response->getLastModified() << std::endl;
	oss << "Allow: " << Util::allowToString(_response->getAllow()) << std::endl;
	oss << "Location: " << _response->getLocation() << std::endl;
	oss << "Retry-After: " << _response->getRetryAfter() << std::endl;
	//oss << "Transfer-Encoding: " << _response->getTransferEncoding() << std::endl;
	oss << "WWW-Authenticate: " << _response->getWWWAuthenticate() << std::endl;
	oss << std::endl;

	// Body
	// if (this->_request->getMethod() != GET && this->_request->getMethod() != HEAD)
	oss << _response->getBody(); // Is it nessecary to add "\n" after last line?

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

	ssize_t		ret;

	if (getWantToRead() == true)
	{
		ret = read(_socket, _buf, BUFFER_SIZE);
		std::cout << "Ret - " << ret << std::endl;
		if (ret < 0)
		{
			std::cout << "Error read\n";
			// Exceptions will be better!
		}
		else if (ret == 0 || ret < BUFFER_SIZE)
		{
			// EOF reached
			_buf[ret] = '\0';
			//_requestStr += _buf;
			int i = 0;
			while (i < ret){
				_requestStr += _buf[i];
				i++;
			}
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
			int i = 0;
			while (i < ret){
				_requestStr += _buf[i];
				i++;
			}
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
