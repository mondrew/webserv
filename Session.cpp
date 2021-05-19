/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/19 12:46:33 by gjessica         ###   ########.fr       */
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

Session::Session(int a_sockfd, int remoteAddr, Server *master) : ASocketOwner(a_sockfd),
																 _theMaster(master),
																 _request(0),
																 _response(0),
																 _cgiRequest(0),
																 _cgiResponse(0),
																 _serverLocation(0),
																 _remoteAddr(remoteAddr),
																 _deleteMe(false),
																 _validRequestFlag(false)
{
}

void Session::handle(int action)
{
	int ret = 0;

	if (action == READ)
	{
		char bufffer[BUFFER_SIZE] = {0};
		ret = recv(_socket, bufffer, BUFFER_SIZE - 1, MSG_DONTWAIT);
		if (ret < 0)
		{
			std::cerr << "Error read\n";
			_deleteMe = true;
			setWantToRead(false);
			setWantToWrite(false);
		}
		else if (ret == 0)
		{
			setWantToRead(false);
			setWantToWrite(true);
		}
		else
		{
			_requestStr += std::string(bufffer);
			if (isEndRequest(_requestStr))
			{
				setWantToRead(false);
				setWantToWrite(true);
				if (Util::printRequests)
					Logger::log("HTTPRequest", _requestStr, TEXT_GREEN);
				Util::printWithTime("END READ SOCKET");
				generateResponse();
				Util::printWithTime("RESPONSE IS GENERATED");
			}
		}
	}
	else if (action == WRITE)
	{
		ret = send(_socket, _responseStr.c_str(), _responseStr.size(), 0);
		if (ret == 0 || ret == -1)
		{
			_deleteMe = true;
			setWantToRead(false);
			setWantToWrite(false);
		}
		else
		{
			_responseStr.erase(0, ret);
			if (_responseStr.empty())
			{
				setWantToRead(true);
				setWantToWrite(false);
				this->clean();
				Util::printWithTime("END SEND RESPONSE");
			}
		}
	}
}

bool Session::isEndRequest(std::string &_requestStr)
{
	if (_requestStr.find("Transfer-Encoding: chunked") != std::string::npos)
		return (_requestStr.find("0\r\n\r\n") != std::string::npos);
	else
		return (_requestStr.find("\r\n\r\n") != std::string::npos);
}

bool Session::isValidRequestTarget(void)
{
	bool isValidPath = false;
	bool isValidFolder = false;
	std::string reqPath = this->_request->getTarget(); // current request path
	std::string fullPath;
	std::string cleanPath;

	if (Util::getLastChar(reqPath) == '/' && reqPath.compare("/") != 0)
		reqPath = Util::removeLastPath(reqPath); // for removing '/' at the end

	// Check is there such Location in the config file
	// For CGI -> create special location in config '/cgi-bin'
	for (std::vector<Location *>::iterator it =
			 this->_theMaster->getLocationSet().begin();
		 it < this->_theMaster->getLocationSet().end(); it++)
	{
		if ((*it)->isContainedInPath(reqPath))
		{

			if (this->_request->getTarget().find(".bla") != std::string::npos)
			{
				if ((*it)->getLocationPath().compare("/") == 0)
					continue;
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
				if ((*it)->getLocationPath().compare("/") != 0 ||
					!Util::isDirectory(fullPath))
					break;
			}
		}
	}
	if (!isValidFolder)
		return (false);

	this->_responseFilePath = fullPath;
	// std::cout << "===>>> FULL PATH: " << fullPath << std::endl; // debug

	if (Util::isDirectory(fullPath))
	{
		std::string delim = "/";
		if (Util::getLastChar(fullPath) == '/')
			delim = "";

		if (!this->_serverLocation->getIndex().empty() &&
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
		std::cout << "RESPONSE FIlE PATH: " << this->_responseFilePath << std::endl;
	}
	return (isValidPath);
}

bool Session::isValidRequestAllow(void) const
{
	return ((this->_serverLocation->getLimitExcept() == 0) ||
			(_request->getMethod() & this->_serverLocation->getLimitExcept()));
}

bool Session::isValidRequestHost(void) const
{
	return (!_request->getHost().empty());
}

bool Session::isValidPermissions(void) const
{
	return (Util::isAllowedToRead(this->_responseFilePath));
}

bool Session::isValidBodySize(void) const
{
	if (this->_serverLocation->getMaxBody() != NOT_LIMIT)
		return (static_cast<long>(this->_request->getBody().size()) <=
				this->_serverLocation->getMaxBody());
	return (true);
}

bool Session::isCGI(void) const
{
	return (Util::isCGI(this->_responseFilePath));
}

void Session::fillDefaultResponseFields(void)
{
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

bool Session::makeErrorResponse(int code)
{
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

	responseToString();
	setWantToWrite(true);
	return (false);
}

bool Session::isValidRequest(void)
{
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

char **Session::createArgv(void)
{

	std::list<std::string> vs;
	std::string str;
	char **argv = 0;
	std::size_t i = 0;

	vs.push_front(this->_responseFilePath);
	if (!this->_request->getQueryString().empty())
	{
		std::istringstream iss(this->_request->getQueryString());
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

	argv = static_cast<char **>(malloc(sizeof(char *) * (vs.size() + 1)));

	for (std::list<std::string>::iterator it = vs.begin(); it != vs.end(); it++)
	{
		argv[i] = static_cast<char *>(malloc(sizeof(char) * ((*it).length() + 1)));
		memcpy(argv[i], (*it).c_str(), (*it).length() + 1);
		i++;
	}
	argv[vs.size()] = 0;
	return (argv);
}

char **Session::createEnvp(CGIRequest *cgiRequest)
{
	std::list<std::string> envpList;
	char **envp;
	std::size_t i = 0;

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
		for (std::map<std::string, std::string>::iterator it =
				 this->_request->getSpecialHeaders().begin();
			 it != this->_request->getSpecialHeaders().end(); it++)
			envpList.push_back("HTTP_" +
							   Util::toUpperUnderscore(it->first) + "=" + it->second);
	}

	envp = static_cast<char **>(malloc(sizeof(char *) * (envpList.size() + 1)));

	for (std::list<std::string>::iterator it = envpList.begin();
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
		int j = 0;
		while (envp[j])
		{
			std::cerr << envp[j] << std::endl;
			j++;
		}
		// std::cerr << "!j!: " << j << std::endl; // debug
		std::cout << "~~~~~~~~~~~~~~~~~~~~~CGI_REQUEST END~~~~~~~~~~~~~~~~~~~~" << std::endl;
	}
	envpList.clear();
	return (envp);
}

void Session::makeCGIResponse(void)
{

	int pid;
	int defStdin;
	int defStdout;
	Util::printWithTime("start makeCGIResponse");
	FILE *fIn = tmpfile();
	FILE *fOut = tmpfile();

	long fdIn = fileno(fIn);
	long fdOut = fileno(fOut);

	defStdin = dup(STDIN_FILENO);
	defStdout = dup(STDOUT_FILENO);

	if (this->_request->getTarget().find(".bla") != std::string::npos &&
		this->_request->getMethod() == POST)
	{
		this->_responseFilePathOld = this->_responseFilePath;
		this->_responseFilePath = this->_serverLocation->getCgiPath();
		this->_request->setCgiPathInfo(this->_request->getTarget()); // IT SHOULD BE CORRECT!!!
	}
	if (this->_request->getContentType().find(
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
	write(fdIn, this->_request->getBody().c_str(), this->_request->getBody().size());
	lseek(fdIn, 0, SEEK_SET);
	char **argv = createArgv();
	char **envp = createEnvp(_cgiRequest);
	Util::printWithTime("START FORK");
	pid = fork();

	if (pid == -1)
	{
		std::cout << "Error pid\n";
	}
	else if (pid == 0)
	{
		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		execve(_responseFilePath.c_str(),
			   const_cast<char *const *>(argv), const_cast<char *const *>(envp));
		std::cerr << "Error execve\n";
		exit(0);
	}
	else
	{
		waitpid(-1, NULL, 0);
		Util::printWithTime("PARENT FORK");
		lseek(fdOut, 0, SEEK_SET);
		int retVal = 1;
		char buffer[BUFFER_SIZE + 1];
		while (retVal > 0)
		{
			memset(buffer, 0, BUFFER_SIZE);
			retVal = read(fdOut, buffer, BUFFER_SIZE);
			buffer[retVal] = '\0';
			_oss << buffer;
		}
		_response->setStatusCode(200);
		_response->setStatusText("OK");
		_response->setAllow(_serverLocation->getLimitExcept());
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
		this->_response->setLastModified(
			Util::getFileLastModified(this->_responseFilePath));

		responseToString();
		setWantToWrite(true);
	}

	for (int i = 0; argv[i]; i++)
		free(const_cast<void *>(static_cast<const void *>(argv[i])));
	free(argv);

	for (int i = 0; envp[i]; i++)
		free(const_cast<void *>(static_cast<const void *>(envp[i])));
	free(envp);
	Util::printWithTime("END makeCGIResponse");

	dup2(defStdin, STDIN_FILENO);
	dup2(defStdout, STDOUT_FILENO);
	fclose(fIn);
	fclose(fOut);
	close(fdIn);
	close(fdOut);
	close(defStdin);
	close(defStdout);
}

void Session::makeRedirectionResponse(std::string const &path,
									  int statusCode, std::string const &statusText)
{
	std::size_t pos;

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

std::string Session::getDirListing(std::string const &path)
{
	DIR *dir;
	struct dirent *diread;
	std::ostringstream oss;
	std::list<std::string> files;

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
		for (std::list<std::string>::iterator it = files.begin();
			 it != files.end(); it++)
		{
			oss << "<a href='" << this->_request->getTarget();
			if (Util::getLastChar(this->_request->getTarget()) != '/')
				oss << "/";
			oss << *it << "'>";
			oss << *it;
			oss << "</a>";
			oss << "<br />";
			// std::cout << "oss: " << oss.str() << std::endl; // debug
		}
		closedir(dir);
	}
	else
	{
		// Internal Server error
		Logger::e("Error open directory " + path);
		return "";
	}
	return (oss.str());
}

std::string generateFilename()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;

	std::stringstream ss;
	ss << ms;
	std::string res = ss.str();

	return res;
}

std::string createFileUpload(std::string const &ext, std::string const &body)
{

	std::string filename = generateFilename();
	std::string tmp = "./www/upload/" + filename + "." + ext;
	std::ofstream outfile(tmp.c_str());

	outfile << body;
	outfile.close();
	return (filename + "." + ext);
}

void Session::makeGETResponse(void)
{

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
	_response->setContentType(
		Util::detectContentType(this->_responseFilePath));
	_response->setLastModified(
		Util::getFileLastModified(this->_responseFilePath));

	responseToString();
	setWantToWrite(true);
}

void Session::makePOSTResponse(void)
{

	_response->setStatusCode(200);
	_response->setStatusText("OK");
	_response->setAllow(_serverLocation->getLimitExcept());
	// _response->setTransferEncoding(_request->getTransferEncoding());
	// _response->setLocation(this->_responseFilePath);

	if (!_request->getBody().empty())
	{
		_response->setStatusCode(201);
		_response->setStatusText("Created");

		std::string conType = _request->getContentType();
		std::string filePath = "./www/upload/" +
							   createFileUpload(Util::getTypeByMime(conType), _request->getBody());
		_response->setLocation(filePath);
	}

	responseToString();
	setWantToWrite(true);
	//std::cout << "MAKE POST RESPONSE!!! END<========!!!" << std::endl; // debug
}

void Session::makePUTResponse(void)
{

	//std::cout << _request->getFilename() << " | "<< _serverLocation->getRoot() << std::endl;
	std::ofstream outfile((_serverLocation->getRoot() +
						   "/upload/" + _request->getFilename())
							  .c_str());
	outfile << _request->getBody();
	outfile.close();

	_response->setStatusCode(201);
	_response->setStatusText("Created");
	_response->setAllow(_serverLocation->getLimitExcept());
	_response->setLocation(this->_responseFilePath);
	// _response->setTransferEncoding(_request->getTransferEncoding());

	responseToString();
	setWantToWrite(true);
}

void Session::generateResponse(void)
{
	Util::printWithTime("START GENERATE RESPONSE");
	// std::cerr << "Generate Response!" << std::endl; // endl;
	if (!this->_request && !this->_response)
	{
		this->_request = new HTTPRequest(_requestStr, this);
		Util::printWithTime("HTTPREquest GENERATED");
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

void Session::responseToString(void)
{
	std::ostringstream oss;

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
	oss.clear();

	if (Util::printResponses)
		Logger::log("Response", _responseStr, TEXT_RED);
}

void Session::setRequestCgiPathTranslated(void) const
{
	if (!this->_request->getCgiPathInfo().empty() &&
		this->_request->getCgiPathTranslated().empty())
		this->_request->setCgiPathTranslated();
}

void Session::remove(void)
{
	_theMaster->removeSession(this);
}

bool Session::isDeleteMe(void) const
{
	return (this->_deleteMe);
}

Session::~Session(void)
{
	if (_request)
		delete _request;
	if (_cgiRequest)
		delete _cgiRequest;
	if (_cgiResponse)
		delete _cgiResponse;
	if (_response)
		delete _response;
	return;
}

void Session::clean()
{
	if (_request)
	{
		delete _request;
		_request = 0;
	}
	if (_response)
	{
		delete _response;
		_response = 0;
	}
	if (_cgiRequest)
	{
		delete _cgiRequest;
		_cgiRequest = 0;
	}
	if (_cgiResponse)
	{
		delete _cgiResponse;
		_cgiResponse = 0;
	}
	_serverLocation = 0;
	_validRequestFlag = false;
	_requestStr = "";
	_responseStr = "";
	_responseFilePath = "";
	_responseFilePathOld = "";
	_login = "";
	_password = "";
	_validRequestFlag = false;
	_oss.str("");
	_oss.clear();
}

// GETTERS

int Session::getRemoteAddr(void) const
{
	return (this->_remoteAddr);
}

Location *Session::getServerLocation(void) const
{
	return (this->_serverLocation);
}

std::string const &Session::getResponseFilePath(void) const
{

	return (this->_responseFilePath);
}

// SETTERS
