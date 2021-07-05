#include "HTTPRequest.hpp"
#include "Util.hpp"
#include <locale>
#include <cstdlib>
#include <algorithm>
#include <utility>

HTTPRequest::HTTPRequest(std::string &str, Session *session) :
													_session(session),
													_valid(true),
													_allow(0) {
	parseRequest(str);
}

HTTPRequest::~HTTPRequest(void) {

	return ;
}

bool			HTTPRequest::isValid(void) const {

	return (this->_valid);
}


int				HTTPRequest::setError(std::string const &str) {

	this->_error = str;
	this->_valid = false;
	return (1);
}

void			HTTPRequest::setBody(std::string const &str) {

	this->_body = str;
}

std::string		toLower(std::string const &str) {

	int		len = str.length();
	int		i = 0;
	char	res[len + 1];

	while (i < len)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
			res[i] = str[i] - ('Z' - 'z');
		else
			res[i] = str[i];
		i++;
	}
	res[i] = '\0';
	return (res);
}

int 			isKey(std::string const &line, std::string const &key) {

	return ((toLower(line)).find(toLower(key)) == 0);
}

std::string		getValue(std::string const &line, std::string const &keyIn) {

	std::string		str = toLower(line);
	std::string		key = toLower(keyIn);
	std::size_t		pos = 0;

	str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
	if ((pos = str.find(key)) != std::string::npos && \
											str.length() > pos + key.length())
		pos += key.length();
	else
		return ("");
	while (str[pos] == ':' || str[pos] == ' ')
		pos++;
	if (str.length() > pos)
		return (str.substr(pos));
	else
		return ("");
}

std::pair<std::string, std::string>	getSpecialHeader(std::string const &line) {

	std::string		header;
	std::string		value;
	std::size_t		pos;

	if ((pos = line.find(":")) != std::string::npos)
	{
		header = line.substr(0, pos);
		if (line.length() > pos + 1)
			value = line.substr(pos + 1);
		else
			value = "";
		value = Util::removeLeadingWhitespaces(value);
	}
	return (std::pair<std::string, std::string>(header, value));
}

bool 			HTTPRequest::setStartLineParam(std::string &line) {

	std::string		tmpTarget;
	std::size_t		pos;

	if (isKey(line, "GET"))
		this->_method = GET;
	else if (isKey(line, "POST"))
		this->_method = POST;
	else if (isKey(line, "PUT"))
		this->_method = PUT;
	else if (isKey(line, "HEAD"))
		this->_method = HEAD;
	else
	{
		this->_method = UNKNOWN;
		return (false);
	}
	if ((pos = line.find("/")) != std::string::npos)
		line = line.substr(pos);
	else
		return (false);
	if ((pos = line.find(" ")) != std::string::npos)
		tmpTarget = line.substr(0, pos);
	else
		return (false);
	// Find query string
	if ((pos = tmpTarget.find("?")) != std::string::npos)
	{
		// There is a query string in the GET or POST request
		this->_target = tmpTarget.substr(0, pos);
		this->_queryString = tmpTarget.substr(pos + 1);
	}
	else
		this->_target = tmpTarget;
	// Find cgiPathInfo & cgiPathTranslated in _target
	if (Util::isPathInfo(this->_target))
		splitTargetAndCgiPathInfo();
	if (this->_method == PUT)
	{
		if ((pos = this->_target.find_last_of("/")) != std::string::npos)
		{
			if (this->_target.length() > pos + 1)
			{
				this->_filename = this->_target.substr(pos + 1);
				this->_target.erase(pos);
			}
			else
				return (false);
		}
		else
			return (false);
	}

	if ((pos = line.find(" ")) != std::string::npos)
	{
		if (line.length() > pos + 1)
			this->_protocolVersion = line.substr(pos + 1);
		else
			return (false);
	}
	else
		return (false);

	if (this->_target.empty() || this->_protocolVersion.empty())
		return (false);
	return (true);
}

void			HTTPRequest::parseRequest(std::string &str) {

	std::istringstream	f(str);
	std::string			line;
	int lineSize = 0;

	//Check first line: #METHOD #PATH #PROTOCOL
	std::getline(f, line);
	lineSize = line.length() + 1;
	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
	if (!setStartLineParam(line))
		return ;
	str.erase(0, lineSize );

    while (std::getline(f, line))
	{
		lineSize = line.length() + 1;
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		if (isKey(line, "Accept-Charsets"))
			this->_acceptCharset = getValue(line, "Accept-Charsets");
		else if (isKey(line, "Accept-Language"))
			this->_acceptLanguage = getValue(line, "Accept-Language");
		else if (isKey(line, "Allow"))
		{
			if (line.find("GET") == 0)
				this->_allow += GET;
			if (line.find("POST") == 0)
				this->_allow += POST;
			if (line.find("PUT") == 0)
				this->_allow += PUT;
			if (line.find("HEAD") == 0)
				this->_allow += HEAD;
		}
		else if (isKey(line, "Authorization"))
			this->_authorization = getValue(line, "Authorization");
		else if (isKey(line, "Content-Language"))
			this->_contentLanguage = getValue(line, "Content-Language");
		else if (isKey(line, "Content-Length") )
			this->_contentLength = atoi(getValue(line, "Content-Length").c_str());
		else if (isKey(line, "Content-Location"))
			this->_contentLocation = getValue(line, "Content-Location");
		else if (isKey(line, "Content-Type") )
			this->_contentType = getValue(line, "Content-Type");
		else if (isKey(line, "Date"))
			this->_date = getValue(line, "Date");
		else if (isKey(line, "Host"))
			this->_host = getValue(line, "Host");
		else if (isKey(line, "Referer"))
			this->_referer = getValue(line, "Referer");
		else if (isKey(line, "User-Agent"))
			this->_userAgent = getValue(line, "User-Agent");
		else if (isKey(line, "Transfer-Encoding"))
			this->_transferEncoding = getValue(line, "Transfer-Encoding");
		else if (line.empty())
		{
			str.erase(0, lineSize);
			this->_body = str;
			break;
		}
		else
			this->_specialHeaders.insert(getSpecialHeader(line));
		str.erase(0, lineSize);
    }
	if (this->_transferEncoding == "chunked")
		this->_body = Util::unchunkData(this->_body);
}

void			HTTPRequest::print(void) const {

	std::cout << "Methods = ";
	if (this->_method == UNKNOWN)
	{
		std::cout << "default\n";
	}
	else
	{
		if (this->_method & GET) std::cout << "GET";
	 	if (this->_method & POST) std::cout << "POST";
	 	if (this->_method & PUT) std::cout << "PUT";
	 	if (this->_method & HEAD) std::cout << "HEAD";
		std::cout << std::endl;
	}
	std::cout << "Target = " << this->_target << std::endl;
	std::cout << "Protocol = " << this->_protocolVersion << std::endl;
	std::cout << "Accept-Charsets = " << this->_acceptCharset << std::endl;
	std::cout << "Accept-Language = " << this->_acceptLanguage << std::endl;
	std::cout << "Authorization = " << this->_authorization << std::endl;
	std::cout << "Content-Language = " << this->_contentLanguage << std::endl;
	std::cout << "Content-Length = " << this->_contentLength << std::endl;
	std::cout << "Content-Location = " << this->_contentLocation << std::endl;
	std::cout << "Content-Type = " << this->_contentType << std::endl;
	std::cout << "Date = " << this->_date << std::endl;
	std::cout << "Host = " << this->_host << std::endl;
	std::cout << "Referer = " << this->_referer << std::endl;
	std::cout << "TransferEncoding = " << this->_transferEncoding << std::endl;
	std::cout << "Body = " << (this->_body).c_str() << std::endl;
	std::cout << "User-Agent = " << this->_userAgent << std::endl;
}

// GETTERS
std::string const	&HTTPRequest::getError(void) const {

	return (this->_error);
}

Options				HTTPRequest::getMethod(void) const {

	return (this->_method);
}

std::string			HTTPRequest::getMethodName(void) const {

	if (this->_method == GET)
		return ("GET");
	else if (this->_method == HEAD)
		return ("HEAD");
	else if (this->_method == POST)
		return ("POST");
	else if (this->_method == PUT)
		return ("PUT");
	else
		return ("UNKNOWN");
}

std::string const	&HTTPRequest::getTarget(void) const {

	return (this->_target);
}

std::string const	&HTTPRequest::getQueryString(void) const {

	return (this->_queryString);
}

std::string const	&HTTPRequest::getProtocolVersion(void) const {

	return (this->_protocolVersion);
}

std::string const	&HTTPRequest::getAcceptCharset(void) const {

	return (this->_acceptCharset);
}

std::string const	&HTTPRequest::getAcceptLanguage(void) const {

	return (this->_acceptLanguage);
}

char 				HTTPRequest::getAllow(void) const {

	return (this->_allow);
}

std::string const	&HTTPRequest::getAuthorization(void) const {

	return (this->_authorization);
}

std::string 		HTTPRequest::getAuthorizationType(void) const {

	size_t	found = 0;

	if ((found = this->_authorization.find(" ")) != std::string::npos)
		return (this->_authorization.substr(0, found));
	else
		return ("");
}

std::string			HTTPRequest::getAuthorizationData(void) const {

	size_t			found = 0;
	std::string		authData;

	if ((found = this->_authorization.find(" ")) != std::string::npos)
	{
		if (this->_authorization.length() > found + 1)
		{
			authData = this->_authorization.substr(found + 1);
			return (Util::removeLeadingWhitespaces(authData));
		}
		else
			return ("");
	}
	else
		return ("");
}

std::string const	&HTTPRequest::getContentLanguage(void) const {

	return (this->_contentLanguage);
}

int					HTTPRequest::getContentLength(void) const {

	return (this->_contentLength);
}

int					HTTPRequest::getBodyLength(void) const {

	return (this->_body.length());
}

std::string const	&HTTPRequest::getContentLocation(void) const {

	return (this->_contentLocation);
}

std::string const	&HTTPRequest::getContentType(void) const {

	return (this->_contentType);
}

std::string const	&HTTPRequest::getTransferEncoding(void) const {

	return (this->_transferEncoding);
}

std::string const	&HTTPRequest::getDate(void) const {

	return (this->_date);
}

std::string const	&HTTPRequest::getHost(void) const {

	return (this->_host);
}

std::string const	&HTTPRequest::getReferer(void) const {

	return (this->_referer);
}

std::string const	&HTTPRequest::getUserAgent(void) const {

	return (this->_userAgent);
}

std::string const	&HTTPRequest::getBody(void) const {

	return (this->_body);
}

std::string const	&HTTPRequest::getCgiPathInfo(void) const {

	return (this->_cgiPathInfo);
}

std::string const	&HTTPRequest::getCgiPathTranslated(void) const {

	return (this->_cgiPathTranslated);
}

Session				*HTTPRequest::getSession(void) const {

	return (this->_session);
}

std::string const	&HTTPRequest::getFilename(void) const {

	return (this->_filename);
}

std::map<std::string, std::string> &HTTPRequest::getSpecialHeaders(void) {

	return (this->_specialHeaders);
}

void				HTTPRequest::splitTargetAndCgiPathInfo(void) {

	std::string		tmp = this->_target;
	std::size_t		pos;

	if ((pos = this->_target.find("cgi-bin/")) != std::string::npos)
	{
		if (this->_target.length() > pos + 8)
		{
			if ((pos = this->_target.find("/", pos + 8)) != std::string::npos)
			{
				this->_target = tmp.substr(0, pos);
				this->_cgiPathInfo = tmp.substr(pos);
			}
		}
	}
}

// SETTERS
void				HTTPRequest::setCgiPathTranslated(void) {

	this->_cgiPathTranslated = \
			this->_session->getServerLocation()->getRoot() + this->_cgiPathInfo;
}

void				HTTPRequest::setCgiPathTranslated(std::string const &path) {

	this->_cgiPathTranslated = path;
}

void				HTTPRequest::setCgiPathInfo(std::string const &str) {

	this->_cgiPathInfo = str;
}

void				HTTPRequest::setContentLength(int contentLength) {

	this->_contentLength = contentLength;
}

void				HTTPRequest::setTarget(std::string const &target) {

	this->_target = target;
}
