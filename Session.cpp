/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/30 23:47:24 by gjessica         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"

Session::Session(int a_sockfd, int remoteAddr, Server *master) :
														ASocketOwner(a_sockfd),
													 	_theMaster(master),
														_request(0),
														_response(0),
														_cgiRequest(0),
														_cgiResponse(0),
														_serverLocation(0),
														_remoteAddr(remoteAddr),
														_deleteMe(false),
														_validRequestFlag(false),
														_fileCGIRequest(0),
														_fileCGIResponse(0),
														_offset(0),
														_launchChild(false),
														_contentLength(-1),
														_headersLength(-1) {

	return ;
}

void	Session::handle(int action) {

	int ret = 0;

	if (action == READ)
	{
		// std::cout << "handle: READ" << std::endl; // debug
		char buffer[BUFFER_SIZE + 1] = {0};
		memset(buffer, 0, BUFFER_SIZE + 1);
		ret = recv(_socket, buffer, BUFFER_SIZE, 0);
		if (ret < 0)
		{
			std::cerr << "Error read\n";
			_deleteMe = true;
			setWantToRead(false);
			setWantToWrite(false);
		}
		else if (ret == 0)
		{
			_deleteMe = true;
			setWantToRead(false);
			setWantToWrite(false);
		}
		else
		{
			std::string		res(buffer, ret);
			_requestStr += res;

			if ((this->_contentLength != -1 || isHeadersEnd(_requestStr)) && \
													isEndRequest(_requestStr))
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
	else if (action == CGI)
		generateResponse();
}

bool	Session::isEndRequest(std::string const &_requestStr) {

	if (_requestStr.find("Transfer-Encoding: chunked") != std::string::npos)
		return (_requestStr.find("0\r\n\r\n") != std::string::npos);
	else
	{
		if (this->_contentLength != 0)
		{
			if (_requestStr.size() < static_cast<std::size_t>(_headersLength + \
															4 + _contentLength))
				return (false);
			else
				return (true);
		}
		else
			return (true);
	}
}

bool	Session::isHeadersEnd(std::string const &_requestStr) {

	std::size_t		pos = 0;

	if ((pos = _requestStr.find("\r\n\r\n")) != std::string::npos)
	{
		_headersLength = pos;
		if ((pos = (Util::toLower(_requestStr)).find("content-length:")) != \
															std::string::npos)
		{
			if (_requestStr.size() > pos + 15)
			{
				this->_contentLength = \
									atoi(_requestStr.substr(pos + 15).c_str());
			}
		}
		else
			this->_contentLength = 0;
		return (true);
	}
	else
		return (false);
}

bool	Session::isValidRequestTarget(void) {

	bool			isValidPath = false;
	bool			isValidFolder = false;
	std::string		reqPath = this->_request->getTarget();
	std::string 	fullPath;
	std::string 	cleanPath;

	if (Util::getLastChar(reqPath) == '/' && reqPath.compare("/") != 0)
		reqPath = Util::removeLastPath(reqPath);

	// Check is there such Location in the config file
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
					break;
			}
		}
	}
	if (!isValidFolder)
		return (false);
	this->_responseFilePath = fullPath;
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
		}
		isValidPath = true;
	}
	else
	{
		if (!Util::exists(fullPath))
			return (false);
		isValidPath = true;
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
		std::cout << "RESPONSE FIlE PATH: " << this->_responseFilePath;
	   	std::cout << std::endl;
	}
	return (isValidPath);
}

bool	Session::isValidRequestAllow(void) const {

	return ((this->_serverLocation->getLimitExcept() == 0) || \
			(_request->getMethod() & this->_serverLocation->getLimitExcept()));
}

bool	Session::isValidRequestHost(void) const {

	size_t pos = 0;
	if ((pos = _request->getHost().find(':')) == std::string::npos)
		pos = _request->getHost().size();
	return (_theMaster->getHost() == _request->getHost().substr(0, pos));
}

bool	Session::isValidPermissions(void) const {

	if (this->_request->getTarget().find(".bla") != std::string::npos)
		return true;
	return (Util::isAllowedToRead(this->_responseFilePath));
}

bool	Session::isValidBodySize(void) const {

	if (this->_serverLocation->getMaxBody() != NOT_LIMIT)
		return (static_cast<long>(this->_request->getBody().size()) <= \
										this->_serverLocation->getMaxBody());
	return (true);
}

bool	Session::isCGI(void) const {

	return (Util::isCGI(this->_responseFilePath));
}

void	Session::fillDefaultResponseFields(void) {

	_response->setProtocolVersion("HTTP/1.1");
	_response->setDate(Util::getDate());
	_response->setServer("MGINX Webserv/1.0 (Unix)");
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

bool	Session::makeErrorResponse(int code) {

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

bool	Session::isValidRequest(void) {

	if (_request->getMethod() == UNKNOWN)
		return (makeErrorResponse(501));
	else if (!_request->isValid() || !this->isValidRequestHost())
		return (makeErrorResponse(400));
	else if (!isValidRequestTarget())
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

char	**Session::createArgv(void) {

	std::list<std::string>	vs;
	std::string				str;
	char					**argv = 0;
	std::size_t				i = 0;

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

char	**Session::createEnvp(CGIRequest *cgiRequest) {

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
		envp[i] = static_cast<char *>(malloc(sizeof(char) * \
														((*it).length() + 1)));
		memcpy(envp[i], (*it).c_str(), (*it).length() + 1);
		i++;
	}
	envp[envpList.size()] = 0;

	if (Util::printCGIRequestENVP)
	{
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~CGI_REQUEST";
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
		int j = 0;
		while (envp[j])
		{
			std::cerr << envp[j] << std::endl;
			j++;
		}
		std::cout << "~~~~~~~~~~~~~~~~~~~~~CGI_REQUEST END";
		std::cout << "~~~~~~~~~~~~~~~~~~~~" << std::endl;
	}
	envpList.clear();
	return (envp);
}

void	Session::makeCGIResponse(void)
{
	if (!getWantToWriteCGIRequest() && !getWantToReadCGIResponse())
	{
		// Only first time
		setWantToWrite(false);

		this->_fileCGIRequest = tmpfile();
		this->_fileCGIResponse = tmpfile();

		this->_fdCGIRequest = fileno(this->_fileCGIRequest);
		this->_fdCGIResponse = fileno(this->_fileCGIResponse);

		if (this->_request->getTarget().find(".bla") != std::string::npos && \
											this->_request->getMethod() == POST)
		{
			this->_responseFilePathOld = this->_responseFilePath;
			this->_responseFilePath = this->_serverLocation->getCgiPath();
			this->_request->setCgiPathInfo(this->_request->getTarget());
		}
		if (this->_request->getContentType().find(
						"application/x-www-form-urlencoded") != std::string::npos)
		{
			this->_request->setBody(Util::decodeUriEncoded(this->_request->getBody()));
			this->_request->setContentLength(this->_request->getBody().length());
		}
		this->_cgiRequest = new CGIRequest(this, this->_request);
		this->_cgiResponse = new CGIResponse();

		if (!this->_request->getBody().empty())
		{
			setWantToWriteCGIRequest(true);
			this->_msgForCGI = this->_request->getBody();
		}
		else
			setWantToReadCGIResponse(true);
		if (Util::printCGIRequest)
		{
			std::cout << "~~~~~~~~~~~~~~~~~~~~~~CGI_REQUEST";
			std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
			this->_cgiRequest->print();
			std::cout << "~~~~~~~~~~~~~~~~~~~~~CGI_REQUEST END";
			std::cout << "~~~~~~~~~~~~~~~~~~~~" << std::endl;
		}
	}
	if (getWantToWriteCGIRequest())
	{
		if (!this->_msgForCGI.empty())
		{
			lseek(this->_fdCGIRequest, this->_offset, SEEK_SET);
			int ret = write(this->_fdCGIRequest, \
							this->_msgForCGI.c_str(), this->_msgForCGI.size());

			if (ret == -1 || ret == 0)
				std::cerr << "Error: write to CGI failed" << std::endl;
			else if (static_cast<std::size_t>(ret) < this->_msgForCGI.size())
			{
				this->_msgForCGI.erase(0, ret);
				this->_offset += ret;
			}
			else
			{
				setWantToWriteCGIRequest(false);
				setWantToReadCGIResponse(true);
				lseek(this->_fdCGIRequest, 0, SEEK_SET);
				this->_offset = 0;
			}
		}
		else
		{
			setWantToWriteCGIRequest(false);
			setWantToReadCGIResponse(true);
			lseek(this->_fdCGIRequest, 0, SEEK_SET);
			this->_offset = 0;
		}
	}
	if (getWantToReadCGIResponse())
	{
		if (!this->_launchChild)
		{
			this->_launchChild = true;
			if ((this->_pid = fork()) == -1)
			{
				std::cerr << "Error pid\n";
				setWantToReadCGIResponse(false);
				setWantToRead(true);
				setWantToWrite(false);
				return ;
			}
			else if (this->_pid == 0)
			{
				// Child
				char	**argv = createArgv();
				char	**envp = createEnvp(_cgiRequest);
				dup2(this->_fdCGIRequest, STDIN_FILENO);
				dup2(this->_fdCGIResponse, STDOUT_FILENO);
				execve(_responseFilePath.c_str(), \
					   				const_cast<char *const *>(argv), \
						   					const_cast<char *const *>(envp));
				std::cerr << "Error execve\n";
				exit(0);
			}
			// Parent
			waitpid(-1, NULL, 0);
		}
		// Parent
		int		retVal = 0;
		char	buffer[BUFFER_SIZE + 1];

		memset(buffer, 0, BUFFER_SIZE + 1);
		lseek(this->_fdCGIResponse, this->_offset, SEEK_SET);

		if ((retVal = read(this->_fdCGIResponse, buffer, BUFFER_SIZE)) == -1)
		{
			std::cerr << "Error: read in CGI fails" << std::endl;
			setWantToReadCGIResponse(false);
			setWantToRead(true);
			setWantToWrite(false);
			return ;
		}
		else if (retVal == 0)
		{
			// EOF reached
			setWantToReadCGIResponse(false);
			setWantToWrite(true);
		}
		else
		{
			this->_readStr += std::string(buffer);
			this->_offset += retVal;
		}
	}
	if (getWantToWrite())
	{
		Util::printWithTime("PARENT FORK after read");
		_response->setStatusCode(200);
		_response->setStatusText("OK");
		_response->setAllow(_serverLocation->getLimitExcept());
		_cgiResponse->parseCGIResponse(_readStr);
		Util::printWithTime("PARENT FORK TEST2");
		if (!this->_cgiResponse->getStatus().empty())
		{
			this->_response->setStatusCode(\
										this->_cgiResponse->getStatusCode());
			this->_response->setStatusText(\
										this->_cgiResponse->getStatusText());
		}
		Util::printWithTime("PARENT FORK TEST1");
		if (!this->_cgiResponse->getContentType().empty())
			this->_response->setContentType(\
										this->_cgiResponse->getContentType());
		else
			this->_response->setContentType("text/html");

		if (!this->_cgiResponse->getBody().empty())
			this->_response->setBody(_cgiResponse->getBody());
		if (this->_cgiResponse->getContentLength() != 0)
			this->_response->setContentLength(\
										this->_cgiResponse->getContentLength());
		else
			this->_response->setContentLength(_cgiResponse->getBody().length());
		this->_response->setContentLocation(this->_responseFilePath);

		this->_response->setLastModified(\
							Util::getFileLastModified(this->_responseFilePath));
		fclose(this->_fileCGIRequest);
		fclose(this->_fileCGIResponse);
		close(this->_fdCGIRequest);
		close(this->_fdCGIResponse);
		setFdCGIRequest(-1);
		setFdCGIResponse(-1);

		responseToString();
		setWantToWrite(true);

		Util::printWithTime("END makeCGIResponse");
	}
}

void	Session::makeRedirectionResponse(std::string const &path,
							  int statusCode, std::string const &statusText) {

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

std::string	Session::getDirListing(std::string const &path) {

	DIR						*dir;
	struct dirent			*diread;
	std::ostringstream		oss;
	std::list<std::string>	files;

	oss << "<html><head><title>MGINX</title></head>";
	oss << "<body>";
	if ((dir = opendir((path).c_str())) != 0)
	{
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
		}
		closedir(dir);
	}
	else
	{
		Logger::e("Error open directory " + path);
		return ("");
	}
	return (oss.str());
}

std::string generateFilename()
{
	std::stringstream	ss;
	struct timeval		tp;
	gettimeofday(&tp, NULL);

	long int			ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;

	ss << ms;
	std::string res = ss.str();

	return (res);
}

std::string createFileUpload(std::string const &ext, std::string const &body) {

	std::string filename = generateFilename();
	std::string tmp = "./www/upload/" + filename + "." + ext;
	std::ofstream outfile(tmp.c_str());

	outfile << body;
	outfile.close();
	return (filename + "." + ext);
}

void	Session::makeGETResponse(void) {

	_response->setStatusCode(200);
	_response->setStatusText("OK");
	_response->setTransferEncoding("identity");
	_response->setContentLanguage("en, en-US, ru");
	_response->setAllow(_serverLocation->getLimitExcept());
	_response->setLocation(this->_responseFilePath);

	// Autoindex
	if (!Util::isDirectory(this->_responseFilePath))
		_response->setBody(Util::fileToString((this->_responseFilePath)));
	else
		_response->setBody(getDirListing(this->_responseFilePath));
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

void	Session::makePOSTResponse(void) {

	_response->setStatusCode(200);
	_response->setStatusText("OK");
	_response->setAllow(_serverLocation->getLimitExcept());

	if (!_request->getBody().empty())
	{
		_response->setStatusCode(201);
		_response->setStatusText("Created");

		std::string		conType = _request->getContentType();
		std::string		filePath = "./www/upload/" + \
		   createFileUpload(Util::getTypeByMime(conType), _request->getBody());
		_response->setLocation(filePath);
	}

	responseToString();
	setWantToWrite(true);
}

void	Session::makePUTResponse(void) {

	std::ofstream	outfile((_serverLocation->getRoot() +
							   "/upload/" + _request->getFilename()).c_str());
	outfile << _request->getBody();
	outfile.close();

	_response->setStatusCode(201);
	_response->setStatusText("Created");
	_response->setAllow(_serverLocation->getLimitExcept());
	_response->setLocation(this->_responseFilePath);

	responseToString();
	setWantToWrite(true);
}

void	Session::generateResponse(void)
{
	//Util::printWithTime("START GENERATE RESPONSE");
	// std::cerr << "Generate Response!" << std::endl; // endl;
	if (!this->_request && !this->_response)
	{
		this->_request = new HTTPRequest(_requestStr, this);
		this->_response = new HTTPResponse();
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

void	Session::responseToString(void) {

	// Status Line
	_responseStr += _response->getProtocolVersion() + " " + Util::toString(_response->getStatusCode());
	_responseStr += " " + _response->getStatusText() + "\r\n";
	// Headers
	_responseStr += "Server: " + _response->getServer() + "\r\n";
	_responseStr += "Date: " + _response->getDate() + "\r\n";
	if (!_response->getContentType().empty())
		_responseStr += "Content-Type: " + _response->getContentType() + "\r\n";
	_responseStr += "Content-Length: " + Util::toString(_response->getContentLength()) + "\r\n";
	if (!_response->getContentLanguage().empty())
		_responseStr += "Content-Language: " + _response->getContentLanguage() + "\r\n";
	if (!_response->getContentLocation().empty())
		_responseStr += "Content-Location: " + _response->getContentLocation() + "\r\n";
	if (!_response->getLastModified().empty())
		_responseStr += "Last-Modified: " + _response->getLastModified() + "\r\n";
	_responseStr += "Allow: " + Util::allowToString(_response->getAllow()) + "\r\n";
	if (!_response->getLocation().empty())
		_responseStr += "Location: " + _response->getLocation() + "\r\n";
	if (!_response->getRetryAfter().empty())
		_responseStr += "Retry-After: " + _response->getRetryAfter() + "\r\n";
	if (!_response->getWWWAuthenticate().empty())
		_responseStr += "WWW-Authenticate: " + _response->getWWWAuthenticate() + "\r\n";
	_responseStr += "\r\n";

	// Body
	if (this->_request->getMethod() != HEAD && !this->_response->getBody().empty())
		_responseStr += _response->getBody();

	if (Util::printResponses)
		Logger::log("Response", _responseStr, TEXT_RED);
}

void	Session::setRequestCgiPathTranslated(void) const {

	if (!this->_request->getCgiPathInfo().empty() &&
		this->_request->getCgiPathTranslated().empty())
		this->_request->setCgiPathTranslated();
}

void	Session::remove(void) {

	_theMaster->removeSession(this);
}

bool	Session::isDeleteMe(void) const {

	return (this->_deleteMe);
}

Session::~Session(void) {

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

void	Session::clean()
{
	// static int k = 0;

	// k++;
	// std::cout << "RESPONSE SENT: " << k << std::endl;
	// if (k == 100097)
	// {
	// 	std::cout << "===================================================================" << std::endl;
	// 	Util::printRequests = true;
	// 	Util::printResponses = true;
	// }

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
	_readStr = "";

	// ASocketOwner
	setWantToRead(true);
	setWantToWrite(false);
	setWantToWriteCGIRequest(false);
	setWantToReadCGIResponse(false);
	_fdCGIRequest = -1;
	_fdCGIResponse = -1;

	_pid = -1;
	_fileCGIRequest = 0;
	_fileCGIResponse = 0;
	_offset = 0;
	_launchChild = false;
	_msgForCGI = "";
	_contentLength = -1;
	_headersLength = -1;
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
