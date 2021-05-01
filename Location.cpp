/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 10:28:50 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/01 22:26:26 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() {

	this->_root = "./www";
	//Change  this->index = "index.html";
	this->_index = "";
	this->_limitExcept = 0;
	this->_autoindex = true;
	this->_maxBody = NOT_LIMIT;
	this->_cgi = "";
	this->_cgiPath = "";
}

Location::~Location() {}

Location::Location(Location const &cp) {

	*this = cp;
}

Location	&Location::operator=(Location const &op) {

	this->_root = op._root;
	this->_index = op._index;
	this->_limitExcept = op._limitExcept;
	this->_autoindex = op._autoindex;
	this->_maxBody = op._maxBody;
	this->_locationPath = op._locationPath;
	this->_cgi = op._cgi;
	this->_cgiPath = op._cgiPath;
	return (*this);
}

bool		Location::isContainedInPath(std::string const &path) const {

	std::size_t		pos;
	std::string		tmp = path;

	// true - only if the 'path' exists in '_locationPath' at the very beginning!
	
	// The special case => path "/"
	if (!this->_locationPath.compare("/"))
		return (path.empty() ? true : false);
	else if ((pos = path.find(this->_locationPath)) == 0)
		return (true);
	return (false);
}

// Getters
long 					Location::getMaxBody() const { return this->_maxBody; }
std::string const		&Location::getRoot() const { return this->_root; }
std::string const		&Location::getIndex() const { return this->_index; }
std::string const		&Location::getCgi() const { return this->_cgi; }
std::string const 		&Location::getCgiPath() const { return this->_cgiPath; }
std::string const		&Location::getLocationPath() const { return this->_locationPath; }
char const				&Location::getLimitExcept() const { return this->_limitExcept; }
bool					Location::isAutoindex() const { return this->_autoindex; }

// Setters
void	Location::setRoot(std::string root) {this->_root = root;}
void	Location::setIndex(std::string index) {this->_index = index;}
void	Location::setMaxBody(long maxBody) { this->_maxBody = maxBody; }
void	Location::setCgi(std::string cgi) { this->_cgi = cgi; }

void	Location::addLimitExcept(Options limitExcept) {
	this->_limitExcept |= limitExcept;
}

void	Location::setAutoindex(bool autoindex) {
	this->_autoindex = autoindex;
}

void	Location::setCgiPath(std::string cgiPath) {
	this->_cgiPath = cgiPath;
}

void	Location::setLocationPath(std::string locationPath) {
	this->_locationPath = locationPath;
}

void  Location::print() const
{
	std::cout << "Location " << this->_locationPath << " {\n";
	std::cout << "root " << this->_root << std::endl;
	std::cout << "client_max_body_size " << this->_maxBody << std::endl;
	std::cout << "limit_except ";
	if (this->_limitExcept == 0)
		std::cout << "default\n";
	else
	{
		if (this->_limitExcept & GET)
			std::cout << "GET";
	 	if (this->_limitExcept & POST)
			std::cout << " POST";
	 	if (this->_limitExcept & PUT)
			std::cout << " PUT";
		std::cout << std::endl;
	}
	std::cout << "index " << this->_index << std::endl;
	std::cout << "autoindex " << this->_autoindex << std::endl;
	std::cout << "cgi " << this->_cgi << std::endl;
	std::cout << "cgi_path " << this->_cgiPath << std::endl;
	std::cout << "}\n";
}
