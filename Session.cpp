/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/09 16:45:44 by mondrew          ###   ########.fr       */
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
#include <cstring>
#define BUFFER 1000000

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

	if (Util::getLastChar(reqPath) == '/' && reqPath.compare("/") != 0)
		reqPath = Util::removeLastPath(reqPath); // for removing '/' at the end
	
	// Check is there such Location in the config file
	// For CGI -> create special location in config '/cgi-bin'
	for (std::vector<Location *>::iterator it = \
						this->_theMaster->getLocationSet().begin();
							it < this->_theMaster->getLocationSet().end(); it++)
	{
		if ((*it)->isContainedInPath(reqPath))
		{

			if (this->_request->getTarget().find(".bla") != std::string::npos)
			{
				if ((*it)->getLocationPath().compare("/") == 0)
					continue ;
				cleanPath = reqPath.substr(((*it)->getLocationPath()).size());
				if (!cleanPath.empty() && cleanPath[0] != '/')
					cleanPath.insert(0, "/");
				fullPath = (*it)->getRoot() + cleanPath;
				this->_responseFilePath = fullPath;
				this->_serverLocation = *it;
				return (true);
			}

			// '/' | '/test.bla' | '/directory'
			cleanPath = reqPath.substr(((*it)->getLocationPath()).size());
			if (!cleanPath.empty() && cleanPath[0] != '/')
				cleanPath.insert(0, "/");
			fullPath = (*it)->getRoot() + cleanPath;

			if (Util::exists(fullPath))
			{
				this->_serverLocation = *it;
				isValidFolder = true;
				if ((*it)->getLocationPath().compare("/") != 0 || \
												!Util::isDirectory(fullPath))
					break ;
			}
			// this->_serverLocation = *it;
			// isValidFolder = true;
		}
		/*
		if ((*it)->isContainedInPath(reqPath))
		{
			std::cout << "==========>!!!!!!!!!!" << std::endl; // debug
			cleanPath = reqPath.erase(0, ((*it)->getLocationPath()).size());
			this->_serverLocation = *it;
			if (!cleanPath.empty() && cleanPath[0] != '/')
				cleanPath.insert(0, "/");
			fullPath = (*it)->getRoot() + cleanPath;
			isValidFolder = true;
			break ;
		}
		*/
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

bool		Session::isValidBodySize(void) const {

	if (this->_serverLocation->getMaxBody() != NOT_LIMIT)
		return (static_cast<long>(this->_request->getBody().size()) <= \
										this->_serverLocation->getMaxBody());
	return (true);
}

bool		Session::isCGI(void) const {

	return (Util::isCGI(this->_responseFilePath));
}

void		Session::fillDefaultResponseFields(void) {

	_response->setProtocolVersion("HTTP/1.1");
	_response->setDate(Util::getDate());
	_response->setServer("MGINX Webserv/1.0 (Unix)");
	// _response->setContentLanguage("en, en-US, ru");

	// It is senseless
	_response->setAllow(0);
	_response->setLastModified("");
	_response->setLocation("");
	_response->setRetryAfter("");
	_response->setTransferEncoding("");
	_response->setWWWAuthenticate("");
	_response->setContentLocation("");
	_response->setContentType("");
	// _response->setContentType("text/html");
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
	else if (code == 413)
		_response->setStatusText("Request Entity Too Large");
	else if (code == 500)
		_response->setStatusText("Internal Server Error");
	else if (code == 501)
		_response->setStatusText("Not Implemented");
	else if (code == 503)
		_response->setStatusText("Service Unavailable");
	else if (code == 505)
		_response->setStatusText("HTTP Version Not Supported");
	_response->setContentType("text/html; charset=utf-8");
	_responseFilePath = (_theMaster->getPagesMap())[code];
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
	else if (!isValidRequestTarget()) // also sets some fields. Not OOP style
		return (fillErrorResponse(404));
	else if (!isValidRequestAllow())
		return (fillErrorResponse(405));
	else if (!isValidPermissions())
		return (fillErrorResponse(403));
	else if (!isValidBodySize())
		return (fillErrorResponse(413));
	return (true);
}

const char			**Session::createArgv(void) {

	std::list<std::string>	vs;
	std::string				str;
	char					**argv = 0;
	std::size_t				i = 0;


	// vs.push_front(this->_request->getTarget());
	vs.push_front(this->_responseFilePath);
	if (!this->_request->getQueryString().empty())
	{
		std::istringstream	iss(this->_request->getQueryString());
		if (this->_request->getQueryString().find("=") != std::string::npos)
		{
			// This is simple query string
			while (std::getline(iss, str, '&'))
				vs.push_back(str);
		}
		else
		{
			// This is search string
			while (std::getline(iss, str, '+'))
				vs.push_back(str);
		}
		// Decode all strings
		for (std::list<std::string>::iterator it = vs.begin(); it != vs.end(); it++)
			*it = Util::decodeUriEncoded(*it);
	}
	if (true) // CGI 06052021 test
	{
	}

	argv = static_cast<char **>(malloc(sizeof(char *) * (vs.size() + 1)));

	for (std::list<std::string>::iterator it = vs.begin(); it != vs.end(); it++)
	{
		argv[i] = static_cast<char *>(malloc(sizeof(char) * ((*it).length() + 1)));
		memcpy(argv[i], (*it).c_str(), (*it).length() + 1);
		i++;
	}
	argv[vs.size()] = 0;
	return (const_cast<const char **>(argv));
}

const char		**Session::createEnvp(CGIRequest *cgiRequest) {

	std::list<std::string>	envpList;
	char					**envp;
	std::size_t				i = 0;

	envpList.push_back(cgiRequest->getAuthType());
	envpList.push_back(cgiRequest->getContentLength());
	envpList.push_back(cgiRequest->getGatewayInterface());
	envpList.push_back(cgiRequest->getPathInfo());
	envpList.push_back(cgiRequest->getPathTranslated());
	envpList.push_back(cgiRequest->getQueryString());
	envpList.push_back(cgiRequest->getRemoteAddr());
	envpList.push_back(cgiRequest->getRemoteIdent());
	envpList.push_back(cgiRequest->getRemoteUser());
	envpList.push_back(cgiRequest->getRequestMethod());
	envpList.push_back(cgiRequest->getRequestURI());
	envpList.push_back(cgiRequest->getScriptName());
	envpList.push_back(cgiRequest->getServerName());
	envpList.push_back(cgiRequest->getServerPort());
	envpList.push_back(cgiRequest->getServerProtocol());
	envpList.push_back(cgiRequest->getServerSoftware());
	if (!this->_request->getSpecialHeaders().empty())
	{
		for (std::map<std::string, std::string>::iterator it = \
						this->_request->getSpecialHeaders().begin(); \
						it != this->_request->getSpecialHeaders().end(); it++)
			envpList.push_back("HTTP_" + \
						Util::toUpperUnderscore(it->first) + "=" + it->second);
	}

	envp = static_cast<char **>(malloc(sizeof(char *) * (envpList.size() + 1)));

	for (std::list<std::string>::iterator it = envpList.begin(); \
													it != envpList.end(); it++)
	{
		envp[i] = static_cast<char *>(malloc(sizeof(char) * ((*it).length() + 1)));
		memcpy(envp[i], (*it).c_str(), (*it).length() + 1);
		i++;
	}
	envp[envpList.size()] = 0;

	// Print ENVP
	if (Util::printCGIRequestENVP)
	{
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~CGI_REQUEST~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
		int		j = 0;
		while (envp[j])
		{
			std::cerr << envp[j] << std::endl;
			j++;
		}
		// std::cerr << "!j!: " << j << std::endl; // debug
		std::cout << "~~~~~~~~~~~~~~~~~~~~~CGI_REQUEST END~~~~~~~~~~~~~~~~~~~~" << std::endl;
	}

	return (const_cast<const char **>(envp));
}

void			Session::makeCGIResponse(void) {

	pid_t				pid;
	int					pipefd[2];
	int					pipefdEx[2];
	std::ostringstream	oss;

	///////////////// NEW ////////// FOR TEST ////////////////////////////////////
	if (this->_request->getTarget().find(".bla") != std::string::npos && \
											this->_request->getMethod() == POST)
	{
		// this->_responseFilePath = "./www/cgi-bin/reader.cgi"; //////////////!!!
		this->_responseFilePathOld = this->_responseFilePath; // save for PATH_INFO
		this->_responseFilePath = this->_serverLocation->getCgiPath();
		// this->_responseFilePath = "./www/cgi-bin/ubuntu_cgi_tester"; //////////////!!!
	}
	///////////////// NEW ////////// FOR TEST ////////////////////////////////////

	if (this->_request->getContentType().find(\
				"application/x-www-form-urlencoded") != std::string::npos)
	{
		this->_request->setBody(Util::decodeUriEncoded(this->_request->getBody()));
		this->_request->setContentLength(this->_request->getBody().length());
	}

	//???/////////////////////////////////////////////////////
	if (!this->getRequestFile().empty())
	{
		// this->_request->setCgiPathInfo((this->_request->getBody()).substr(0, 9)); // 100 billion 'n'
		this->_request->setCgiPathInfo(this->_request->getTarget());// IT SHOULD BE CORRECT!!!

		// this->_request->setCgiPathInfo(this->_responseFilePathOld); // path to 'youpi.bla'

		// this->_request->setCgiPathInfo(""); // path to 'youpi.bla'
		// this->_request->setCgiPathTranslated("./www/YoupiBanane/youpi.bla"); // path to 'youpi.bla'
	}
	///////////////////////////////////////////////////////

	this->_cgiRequest = new CGIRequest(this, this->_request);
	this->_cgiResponse = new CGIResponse();

	if (Util::printCGIRequest)
	{
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~CGI_REQUEST~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
		this->_cgiRequest->print();
		std::cout << "~~~~~~~~~~~~~~~~~~~~~CGI_REQUEST END~~~~~~~~~~~~~~~~~~~~" << std::endl;
	}

	if (pipe(pipefd) == -1)
	{
		std::cout << "Pipe failed" << std::endl;
		// Internal Server Error
	}

	if (pipe(pipefdEx) == -1)
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
		// Child == Child == Child == Child == Child == Child == Child == Child == Child == Child //
		// Close IN pipe side - we should read from stdin
		/*
		if (this->_request->getMethod() == POST)
			dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]); // !!!!!!!!! - IT MAY BE CRITICAL!!!
		*/

		// NEW
		close(pipefdEx[1]);
		dup2(pipefdEx[0], STDIN_FILENO);
		close(pipefdEx[0]);
		// END NEW

		// Make STDOUT be the copy of the pipefd[1] (out pipe end)
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		// 1. Create argv

		const char **argv = createArgv();
		const char **envp = createEnvp(_cgiRequest);

		// 1. [ GET ] method - produce the document based on: meta-variables
		// 		- should parse the query string to the array on words (argv)
		//		- if the query String doesn't contain unencoded '=' character, we should
		//			interpret query string as 'indexed' HTTP query:
		//			a. Parse it (delimiter is '+')
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

		// std::cerr << "Child STARTS EXECVE <+++++++++=====" << std::endl; // debug
		execve(_responseFilePath.c_str(), \
			const_cast<char *const *>(argv), const_cast<char *const *>(envp));

		Util::freeTwoDimentionalArray(argv);
		Util::freeTwoDimentionalArray(envp);

		exit(0);
	}
	// Parent ==  Parent == Parent == Parent == Parent == Parent == Parent == Parent == Parent //

	/* 09/05/2021
	int		saveOut = 98;
	int		saveIn = 99;
	dup2(STDOUT_FILENO, saveOut);
	dup2(STDIN_FILENO, saveIn);

	// NEW
	close(pipefdEx[0]);
	dup2(pipefdEx[1], STDOUT_FILENO);
	close(pipefdEx[1]);
	// END NEW
	
	// Replace stdin with READ-END of the PIPE
	close(pipefd[1]);
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	*/
	////////09/05/2021 NEW
	close(pipefdEx[0]);
	close(pipefd[1]);
	////////09/05/2021 NEW

	int		ret;

	if (!this->_request->getBody().empty())
	{
		/* too old
		std::ifstream	ifs(this->getRequestFile().c_str());
		std::cout << ifs.rdbuf() << std::endl; // I've spent 5 hours fixing this
		close(STDOUT_FILENO); // I've spent 6 hours for that
		ifs.close();
		*/

		// New but with dup2 - ACTUAL
		// std::cout << this->_request->getBody() << std::endl;

		// Without dup2: 09/05/2021
		ret = write(pipefdEx[1], this->_request->getBody().c_str(), \
											this->_request->getBody().size());
		if (ret == -1)
			std::cout << "Write error" << std::endl;
	}

	// std::cerr << "Parent STARTS READING <+++++++++=====" << std::endl; // debug

	/* 09/05/2021
	oss << std::cin.rdbuf();
	oss << std::endl;
	*/

	char	buffer[BUFFER + 1];

	while ((ret = read(pipefd[0], buffer, BUFFER)) > 0)
	{
		buffer[ret] = '\0';
		oss << buffer;
	}
	if (ret < 0)
		std::cout << "Read error" << std::endl;

	// std::cerr << "Parent FINISHED READING <+++++++++=====" << std::endl; // debug

	/* 09/05/2021
	// Restore STDIN_FILENO & STDOUT_FILENO
	dup2(saveOut, STDOUT_FILENO);
	dup2(saveIn, STDIN_FILENO);
	*/
	////////09/05/2021 NEW
	close(pipefdEx[1]);
	close(pipefd[0]);
	////////09/05/2021 NEW

	if (Util::printCGIResponseString) {
		std::cerr << "-----= [ Pure CGI Response String From Child ] =-----" << std::endl;
		std::cerr << "[CGI_STRING]: " << oss.str() << std::endl;
		std::cerr << "-----= [ END CGI Response String ] =-----" << std::endl;
		std::cerr << std::endl;
	}

	_cgiResponse->parseCGIResponse(oss.str());

	if (Util::printCGIResponse)
	{
		std::cout << "================ Parsed CGI Response =====================" << std::endl;
		_cgiResponse->print();
		std::cout << "================ Parsed CGI Response END =====================" << std::endl;
	}
	
	// Check the Redirection cases
	if (!_cgiResponse->getLocation().empty())
	{
		if (_cgiResponse->getLocation().find("http://") == 0)
		{
			// Absolute URI path
			// Client redirect response
			if (!_cgiResponse->getStatus().empty())
			{
				makeRedirectionResponse(_cgiResponse->getLocation(), \
											_cgiResponse->getStatusCode(), \
												_cgiResponse->getStatusText());
			}
			else
				makeRedirectionResponse(_cgiResponse->getLocation(), \
																302, "Found");
			return ;
		}
		else if (_cgiResponse->getBody().empty() && \
											_cgiResponse->getStatus().empty())
		{
			// Local redirect response
			// Redo request with new local URI
			this->_request->setTarget(_cgiResponse->getLocation());
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
				return ;
			}
		}
	}

	if (!this->_cgiResponse->getStatus().empty())
	{
		this->_response->setStatusCode(this->_cgiResponse->getStatusCode());
		this->_response->setStatusText(this->_cgiResponse->getStatusText());
	}
	if (!this->_cgiResponse->getContentType().empty())
		this->_response->setContentType(this->_cgiResponse->getContentType());
	else
		this->_response->setContentType("text/html");

	if (!this->_cgiResponse->getBody().empty())
		this->_response->setBody(_cgiResponse->getBody());
	if (this->_cgiResponse->getContentLength() != 0)
		this->_response->setContentLength(this->_cgiResponse->getContentLength());
	else
		this->_response->setContentLength(_cgiResponse->getBody().length());
	this->_response->setContentLocation(this->_responseFilePath);

	// Change it: get info from CGI
	this->_response->setLastModified(\
				Util::getFileLastModified(this->_responseFilePath));

	// Wait for the child
	waitpid(pid, 0, 0);
}

void			Session::makeRedirectionResponse(std::string const &path, \
								int statusCode, std::string const &statusText) {

	std::size_t		pos;

	this->_response->setStatusCode(statusCode);
	this->_response->setStatusText(statusText);
	this->_response->setLocation(path);
	this->_response->setContentType("text/html; charset=utf-8");

	if (this->_response->getBody().empty())
	{
		this->_responseFilePath = (_theMaster->getPagesMap())[statusCode];
		this->_response->setBody(Util::fileToString((this->_responseFilePath)));
		if ((pos = this->_response->getBody().find("href=\"")) != std::string::npos)
		{
			std::string responseBody = this->_response->getBody();
			this->_response->setBody(responseBody.insert(pos + 6, path));
		}
	}
	else
		this->_response->setBody(_cgiResponse->getBody());
	this->_response->setContentLength(_response->getBody().length());
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
	_response->setContentLanguage("en, en-US, ru");

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

/*
void		Session::makeHEADResponse(void) {

	// do smth
	_response->setStatusCode(200);
	_response->setStatusText("OK");
}
*/

void		Session::makeHEADResponse(void) {

	// HEAD is the same as GET. We decide not to use body in responseToString
	// May be delete this function?
	_response->setStatusCode(200);
	_response->setStatusText("OK");
	_response->setTransferEncoding("identity");
	_response->setContentLanguage("en, en-US, ru");

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

/* GJESSICA
std::string		createFileUpload(std::string const &ext, std::string const &body) {

	std::string		filename = generateFilename();
	std::string 	tmp = "./www/upload/" + filename + "." + ext;
	std::ofstream	outfile(tmp.c_str());

	outfile << body;
	outfile.close();
	return (filename + "." + ext);
}
*/

void		Session::makePOSTResponse(void) {
	// do smth
	// update some info?
	_response->setStatusCode(200);
	_response->setStatusText("OK");
	_response->setTransferEncoding(_request->getTransferEncoding());
	_response->setAllow(_serverLocation->getLimitExcept());
	// _response->setLocation(this->_responseFilePath);
	_response->setBody("");
	_response->setContentLength(0);

	if (!_request->getBody().empty())
	{
		//MONDREW
		/*
		std::ofstream	ofs(_responseFilePath.c_str());

		ofs << _request->getBody();
		ofs.close();
		std::string		conType = _request->getContentType();
		*/

		// GJESSICA
		// std::string		filePath = "./www/upload/" + \
		//	createFileUpload(Util::getTypeByMime(conType), _request->getBody());
		//_requestFile = filePath;
		_requestFile = "1"; // just for makeCGIResponse working at the beginning

		// std::cout << _requestFile << std::endl;
	}
	if (isCGI() || this->_request->getTarget().find(".bla") != std::string::npos) // NEW
		makeCGIResponse();
	else
	{
		_response->setStatusCode(201);
		_response->setStatusText("Created");
		_response->setLocation(_requestFile);
		// std::string conType = _request->getContentType();
		// if (Util::getTypeByMime(conType) != "unk")
		// {
		// 	std::string filename = \
		// 		createFileUpload(Util::getTypeByMime(conType), _request->getBody());
		// 	_response->setStatusCode(201);
		// 	_response->setStatusText("Created");
		// 	_response->setLocation("./www/upload/" + filename);
		// }
	}
}

void		Session::makePUTResponse(void) {

	//std::cout << _request->getFilename() << " | "<< _serverLocation->getRoot() << std::endl;
	std::ofstream	outfile((_serverLocation->getRoot() + \
								"/upload/" + _request->getFilename()).c_str());

	outfile << _request->getBody();
	outfile.close();
	_response->setStatusCode(201);
	_response->setStatusText("Created");
	_response->setTransferEncoding(_request->getTransferEncoding());
	_response->setAllow(_serverLocation->getLimitExcept());
	_response->setLocation(this->_responseFilePath);
	_response->setBody("");
	_response->setContentLength(0);

	if (isCGI())
		makeCGIResponse();
	else
	{
		// std::string conType = _request->getContentType();
		// if (Util::getTypeByMime(conType) != "unk")
		// {
		// 	std::string filename = createFileUpload(Util::getTypeByMime(conType), _request->getBody());
		// 	_response->setStatusCode(201);
		// 	_response->setStatusText("Created");
		// 	_response->setLocation("./www/upload/" + filename);
		// }
	}
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
	oss << " " << _response->getStatusText() << "\r\n";
	// Headers
	oss << "Server: " << _response->getServer() << "\r\n";
	oss << "Date: " << _response->getDate() << "\r\n";
	if (!_response->getContentType().empty())
		oss << "Content-Type: " << _response->getContentType() << "\r\n";
	oss << "Content-Length: " << _response->getContentLength() << "\r\n";
	if (!_response->getContentLanguage().empty())
		oss << "Content-Language: " << _response->getContentLanguage() << "\r\n";
	if (!_response->getContentLocation().empty())
		oss << "Content-Location: " << _response->getContentLocation() << "\r\n";
	if (!_response->getLastModified().empty())
		oss << "Last-Modified: " << _response->getLastModified() << "\r\n";
	oss << "Allow: " << Util::allowToString(_response->getAllow()) << "\r\n";
	if (!_response->getLocation().empty())
		oss << "Location: " << _response->getLocation() << "\r\n";
	if (!_response->getRetryAfter().empty())
		oss << "Retry-After: " << _response->getRetryAfter() << "\r\n";
	//oss << "Transfer-Encoding: " << _response->getTransferEncoding() << "\r\n";
	if (!_response->getWWWAuthenticate().empty())
		oss << "WWW-Authenticate: " << _response->getWWWAuthenticate() << "\r\n";
	oss << "\r\n";

	// Body
	if (this->_request->getMethod() != HEAD && !this->_response->getBody().empty())
		oss << _response->getBody(); // Is it nessecary to add "\n" after last line?

	_responseStr = oss.str();

	if (Util::printResponses)
		Logger::log("Response", _responseStr, TEXT_RED);
}

void		Session::setRequestCgiPathTranslated(void) const {

	if (!this->_request->getCgiPathInfo().empty())
		this->_request->setCgiPathTranslated();
}

void		Session::remove(void) {

	_theMaster->removeSession(this);
}

void		Session::checkNeedToRead()
{
	if (_requestStr.find("Transfer-Encoding: chunked") !=  std::string::npos)
	{
		if (_requestStr.find("0\r\n\r\n") != std::string::npos)
		{
			if (Util::printRequests)
				Logger::log("HTTPRequest", _requestStr, TEXT_GREEN);
			setWantToRead(false);
			setWantToWrite(true);
			generateResponse();
		}
	}
	else if (_requestStr.find("\r\n\r\n") != std::string::npos)
	{
		if (Util::printRequests)
			Logger::log("HTTPRequest", _requestStr, TEXT_GREEN);
		setWantToRead(false);
		setWantToWrite(true);
		generateResponse();
	}
}

// MONDREW handle
void		Session::handle(void) {

	if (Util::printHandleCounter)
	{
		static int j = 0;
		std::cout << "|" << j++ <<  "|" << std::endl;
	}

	ssize_t		ret;
	int 		i = 0;

	if (getWantToRead())
	{
		ret = recv(_socket, _buf, BUFFER_SIZE, 0);
		if (ret < 0)
		{
			std::cout << "Error read\n";
			// Exceptions will be better!
		}
		else
		{
			while (i < ret)
			{
				_requestStr += _buf[i];
				i++;
			}
			checkNeedToRead();
		}
	}
	else if (getWantToWrite())
	{
		if (!_responseStr.empty())
		{
			ret = send(_socket, _responseStr.c_str(), \
				std::min(BUFFER_SIZE, static_cast<int>(_responseStr.length())), 0);
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

// GETTERS

int					Session::getRemoteAddr(void) const {
	return (this->_remoteAddr);
}

Location			*Session::getServerLocation(void) const {
	return (this->_serverLocation);
}

std::string const	&Session::getRequestFile(void) const {
	return (this->_requestFile);
}

std::string const	&Session::getResponseFilePath(void) const {

	return (this->_responseFilePath);
}

// SETTERS

void				Session::setRequestFile(std::string const &requestFile) {
	this->_requestFile = requestFile;
}
