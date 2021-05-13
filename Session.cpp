/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/13 19:31:04 by gjessica         ###   ########.fr       */
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
#include <errno.h>

// Query string added

Session::Session(int a_sockfd, int remoteAddr, Server *master) :
													ASocketOwner(a_sockfd),
													_theMaster(master),
													_request(0),
													_response(0),
													_serverLocation(0),
													_remoteAddr(remoteAddr),
													_deleteMe(false),
													_validRequestFlag(false) {
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
	_response->setContentLength(0);
	_response->setBody("");
}

bool		Session::makeErrorResponse(int code) {

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

	setWantToWrite(true);
	responseToString();
	return (false);
}

bool		Session::isValidRequest(void) {

	//if (!_request->isValid() && _request->getMethod() == UNKNOWN)
	if (_request->getMethod() == UNKNOWN)
		return (makeErrorResponse(501));
	else if (!_request->isValid() || !this->isValidRequestHost())
		return (makeErrorResponse(400));
	else if (!isValidRequestTarget()) // also sets some fields. Not OOP style
		return (makeErrorResponse(404));
	else if (!isValidRequestAllow())
		return (makeErrorResponse(405));
	else if (!isValidPermissions())
		return (makeErrorResponse(403));
	else if (!isValidBodySize())
		return (makeErrorResponse(413));
	this->_validRequestFlag = true;
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

	if (this->_readFd == -1)
	{
		int					pipefd[2];
		int					pipefdEx[2];
		// std::ostringstream	oss;

		if (this->_request->getTarget().find(".bla") != std::string::npos && \
												this->_request->getMethod() == POST)
		{
			this->_responseFilePathOld = this->_responseFilePath;
			this->_responseFilePath = this->_serverLocation->getCgiPath();
			this->_request->setCgiPathInfo(this->_request->getTarget());// IT SHOULD BE CORRECT!!!
		}
		if (this->_request->getContentType().find(\
					"application/x-www-form-urlencoded") != std::string::npos)
		{
			this->_request->setBody(Util::decodeUriEncoded(this->_request->getBody()));
			this->_request->setContentLength(this->_request->getBody().length());
		}

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

		// fcntl(pipefdEx[1], F_SETFL, O_NONBLOCK);


		if ((_pid = fork()) == -1)
		{
			std::cout << "Fork failed" << std::endl;
			// Internal Server Error
		}
		if (_pid == 0)
		{
			// Child == Child == Child == Child == Child == Child == Child == Child == Child == Child //
			// Close IN pipe side - we should read from stdin
			// NEW
			close(pipefdEx[1]);
			dup2(pipefdEx[0], STDIN_FILENO);
			close(pipefdEx[0]);
			// END NEW

			// Make STDOUT be the copy of the pipefd[1] (out pipe end)
			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);

			const char **argv = createArgv();
			const char **envp = createEnvp(_cgiRequest);

			execve(_responseFilePath.c_str(), \
				const_cast<char *const *>(argv), const_cast<char *const *>(envp));

			Util::freeTwoDimentionalArray(argv);
			Util::freeTwoDimentionalArray(envp);
			exit(0);
		}
		// Parent ==  Parent == Parent == Parent == Parent == Parent == Parent == Parent == Parent //
		close(pipefdEx[0]);
		close(pipefd[1]);

		this->_writeFd = pipefdEx[1];
		this->_readFd = pipefd[0];

		if (!this->_request->getBody().empty())
			this->_wantToWriteToFd = true;
		else
			this->_wantToReadFromFd = true;
	}
	else if (getWantToReadFromFd() == true)
	{
		if (this->_request->getBody().empty())
			setWantToWriteToFd(false);
		int		retVal;
		char	buffer[BUFFER_SIZE + 1];
		std::cerr << "getWantToReadFromFd\n";
		fcntl(this->_readFd,F_SETFL, O_NONBLOCK);
		retVal = read(this->_readFd, buffer, BUFFER_SIZE);
		if (retVal < 0)
		{
			setWantToReadFromFd(false);
			std::cerr << "Error read\n";
			// Exceptions will be better!
		}
		else if (retVal > 0)
		{
				std::cerr << "retVal = " << retVal << std::endl;
			// Попробуем прочитать так
			buffer[retVal] = '\0';
			_oss << buffer;
		}
		else
			setWantToReadFromFd(false);

	}
	else if (getWantToWriteToFd() == true)
	{
		 std::cerr << "Writing to CGI script" << std::endl; // debug
		int		ret;

		// Think this std::min is not necessary
		fcntl(this->_writeFd,F_SETFL, O_NONBLOCK);
		ret = write(this->_writeFd, this->_request->getBody().c_str(), \
											this->_request->getBody().size());
		 std::cerr << "ret: " << ret << std::endl; // debug

		if (ret > 0)
		{
			setWantToReadFromFd(true);
			if (ret < static_cast<int>(this->_request->getBody().size()))
			{

				std::cerr << "Continue reading. Decreasing body" << std::endl; // debug
				this->_request->setBody(this->_request->getBody().substr(ret));
				std::cerr << "rest body size: " << this->_request->getBody().size() << std::endl; // debug
			}
			else
			{
				// std::cerr << "Finished reading" << std::endl; // debug
				this->_request->setBody("");
				this->_wantToWriteToFd = false;
				this->_wantToReadFromFd = true;
			}
		}
		else if (ret == -1)
			std::cerr << "Write error" << std::endl;
		else if (ret == 0)
			std::cerr << "ERROR UNKNOWN: write returned 0!!!!<==========" << std::endl;
	}

	if (!getWantToWriteToFd() && !getWantToReadFromFd())
	{
		std::cerr << "!getWantToWriteToFd() && !getWantToReadFromFd()" << std::endl;
		_response->setStatusCode(200);
		_response->setStatusText("OK");
		_response->setAllow(_serverLocation->getLimitExcept());

		// Close resources
		close(this->_writeFd);
		close(this->_readFd);

		_cgiResponse->parseCGIResponse(_oss.str());

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
		// May be it will block???
		waitpid(_pid, 0, 0);

		setWantToWrite(true);
		responseToString();
	}
	if (Util::printCGIResponseString) {
		std::cerr << "-----= [ Pure CGI Response String From Child ] =-----" << std::endl;
		std::cerr << "[CGI_STRING]: " << _oss.str() << std::endl;
		std::cerr << "-----= [ END CGI Response String ] =-----" << std::endl;
		std::cerr << std::endl;
	}
	if (Util::printCGIResponse)
	{
		std::cout << "================ Parsed CGI Response =====================" << std::endl;
		_cgiResponse->print();
		std::cout << "================ Parsed CGI Response END =====================" << std::endl;
	}

	// Check the Redirection cases
	/*
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
	*/
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

std::string		createFileUpload(std::string const &ext, std::string const &body) {

	std::string		filename = generateFilename();
	std::string 	tmp = "./www/upload/" + filename + "." + ext;
	std::ofstream	outfile(tmp.c_str());

	outfile << body;
	outfile.close();
	return (filename + "." + ext);
}

void		Session::makeGETResponse(void) {

	// Set status code & test
	_response->setStatusCode(200);
	_response->setStatusText("OK");
	_response->setTransferEncoding("identity");
	_response->setContentLanguage("en, en-US, ru");
	_response->setAllow(_serverLocation->getLimitExcept());
	_response->setLocation(this->_responseFilePath);

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

	setWantToWrite(true);
	responseToString();
}

void		Session::makePOSTResponse(void) {

	_response->setStatusCode(200);
	_response->setStatusText("OK");
	_response->setAllow(_serverLocation->getLimitExcept());
	// _response->setTransferEncoding(_request->getTransferEncoding());
	// _response->setLocation(this->_responseFilePath);

	if (!_request->getBody().empty())
	{
		_response->setStatusCode(201);
		_response->setStatusText("Created");

		std::string		conType = _request->getContentType();
		std::string		filePath = "./www/upload/" + \
			createFileUpload(Util::getTypeByMime(conType), _request->getBody());
		_response->setLocation(filePath);
	}

	setWantToWrite(true);
	responseToString();
	std::cout << "MAKE POST RESPONSE!!! END<========!!!" << std::endl; // debug
}

void		Session::makePUTResponse(void) {

	//std::cout << _request->getFilename() << " | "<< _serverLocation->getRoot() << std::endl;
	std::ofstream	outfile((_serverLocation->getRoot() + \
								"/upload/" + _request->getFilename()).c_str());
	outfile << _request->getBody();
	outfile.close();

	_response->setStatusCode(201);
	_response->setStatusText("Created");
	_response->setAllow(_serverLocation->getLimitExcept());
	_response->setLocation(this->_responseFilePath);
	// _response->setTransferEncoding(_request->getTransferEncoding());

	setWantToWrite(true);
	responseToString();
}

void		Session::generateResponse(void) {

	// std::cerr << "Generate Response!" << std::endl; // endl;
	if (!this->_request && !this->_response)
	{
		this->_request = new HTTPRequest(_requestStr, this);
		this->_response = new HTTPResponse();

		// Set common fields (independent of response type)
		fillDefaultResponseFields();
	}
	if (_validRequestFlag || isValidRequest())
	{
		setRequestCgiPathTranslated();
		if (isCGI())
			makeCGIResponse();
		else if (_request->getMethod() == GET || _request->getMethod() == HEAD)
			makeGETResponse();
		else if (_request->getMethod() == POST)
		{
			if (this->_request->getTarget().find(".bla") != std::string::npos)
				makeCGIResponse();
			else
				makePOSTResponse();
		}
		else if (_request->getMethod() == PUT)
			makePUTResponse();
	}
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

	if (!this->_request->getCgiPathInfo().empty() && \
								this->_request->getCgiPathTranslated().empty())
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
			setWantToWrite(false);
			// setWantToWrite(true);
			// generateResponse();
		}
	}
	else if (_requestStr.find("\r\n\r\n") != std::string::npos)
	{
		if (Util::printRequests)
			Logger::log("HTTPRequest", _requestStr, TEXT_GREEN);
		setWantToRead(false);
		setWantToWrite(false);
		// setWantToWrite(true);
		// generateResponse();
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
			std::cerr << "Error read\n";
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
	if (!getWantToRead() && !getWantToWrite())
	{
		// Generate Response
		generateResponse();
		// After finishing generate Response -> set wantToWrite!!!
	}
	else if (getWantToWrite())
	{
		if (!_responseStr.empty())
		{
			std::cerr << "write\n";
			ret = send(_socket, _responseStr.c_str(), \
													_responseStr.length(), 0);
			std::cerr << "write END\n";
				//std::min(BUFFER_SIZE, static_cast<int>(_responseStr.length())), 0);
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

std::string const	&Session::getResponseFilePath(void) const {

	return (this->_responseFilePath);
}

// SETTERS
