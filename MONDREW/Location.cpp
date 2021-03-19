/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 10:28:50 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/19 08:52:16 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() {

	this->root = "./www";
	this->index = "index.html";
	this->limitExcept.push_back(LE_GET);
	this->limitExcept.push_back(LE_POST);
	this->autoindex = true;
	this->maxBody = NOT_LIMIT;
	this->cgi = "";
	this->cgi_index = "";
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
	this->cgi_index = op.cgi_index;
	return (*this);
}


// Getters
long 					Location::getMaxBody() const { return this->maxBody; }
std::string const		&Location::getRoot() const { return this->root; }
std::string const		&Location::getIndex() const { return this->index; }
std::string const		&Location::getCgi() const { return this->cgi; }
std::string const 		&Location::getCgi_index() const { return this->cgi_index; }
std::string const		&Location::getLocationPath() const { return this->locationPath; }
std::vector<int> const	&Location::getLimitExcept() const { return this->limitExcept; }
bool					Location::isAutoindex() const { return this->autoindex; }

// Setters
void	Location::setRoot(std::string root) {this->root = root;}
void	Location::setIndex(std::string index) {this->index = index;}
void	Location::setMaxBody(long maxBody) { this->maxBody = maxBody; }
void	Location::setCgi(std::string cgi) { this->cgi = cgi; }
void	Location::setLimitExcept(std::vector<int> limitExcept) {
	this->limitExcept = limitExcept;
}
void	Location::setAutoindex(bool autoindex) {
	this->autoindex = autoindex;
}
void	Location::setCgi_index(std::string cgi_index) {
	this ->cgi_index = cgi_index;
}

void	Location::setLocationPath(std::string locationPath) {
	this->locationPath = locationPath;
}
