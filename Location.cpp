/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 10:28:50 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/31 13:33:09 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() {

	this->root = "./www";
	this->index = "index.html";
	this->limitExcept = 0;
	this->autoindex = true;
	this->maxBody = NOT_LIMIT;
	this->cgi = "";
	this->cgiPath = "";
}

Location::~Location() {}

Location::Location(Location const &cp) {

	*this = cp;
}

Location & Location::operator=(Location const &op) {

	this->root = op.root;
	this->index = op.index;
	this->limitExcept = op.limitExcept;
	this->autoindex = op.autoindex;
	this->maxBody = op.maxBody;
	this->locationPath = op.locationPath;
	this->cgi = op.cgi;
	this->cgiPath = op.cgiPath;
	return (*this);
}

// Getters
long 					Location::getMaxBody() const { return this->maxBody; }
std::string const		&Location::getRoot() const { return this->root; }
std::string const		&Location::getIndex() const { return this->index; }
std::string const		&Location::getCgi() const { return this->cgi; }
std::string const 		&Location::getCgiPath() const { return this->cgiPath; }
std::string const		&Location::getLocationPath() const { return this->locationPath; }
char const				&Location::getLimitExcept() const { return this->limitExcept; }
bool					Location::isAutoindex() const { return this->autoindex; }

// Setters
void	Location::setRoot(std::string root) {this->root = root;}
void	Location::setIndex(std::string index) {this->index = index;}
void	Location::setMaxBody(long maxBody) { this->maxBody = maxBody; }
void	Location::setCgi(std::string cgi) { this->cgi = cgi; }
void	Location::addLimitExcept(Options limitExcept) {
	this->limitExcept |= limitExcept;
}
void	Location::setAutoindex(bool autoindex) {
	this->autoindex = autoindex;
}
void	Location::setCgiPath(std::string cgiPath) {
	this ->cgiPath = cgiPath;
}

void	Location::setLocationPath(std::string locationPath) {
	this->locationPath = locationPath;
}

void  Location::print() const
{
	std::cout << "Location " << this->locationPath << " {\n";
	std::cout << "root " << this->root << std::endl;
	std::cout << "client_max_body_size " << this->maxBody << std::endl;
	std::cout << "limit_except ";
	if (this->limitExcept == 0)
		std::cout << "default\n";
	else
	{
		if (this->limitExcept & GET) std::cout << "GET";
	 	if (this->limitExcept & POST) std::cout << " POST";
	 	if (this->limitExcept & PUT) std::cout << " PUT";
		std::cout << std::endl;
	}
	std::cout << "index " << this->index << std::endl;
	std::cout << "autoindex " << this->autoindex << std::endl;
	std::cout << "cgi " << this->cgi << std::endl;
	std::cout << "cgi_path " << this->cgiPath << std::endl;
	std::cout << "}\n";
}
