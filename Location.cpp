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

Location::Location(Location const &cp){
	*this = cp;
}

Location & Location::operator=(Location const &op){
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

std::string Location::getRoot(){return this->root;}
void Location::setRoot(std::string root){this->root = root;}
std::string Location::getIndex(){return this->index;}
void Location::setIndex(std::string index){this->index = index;}
std::vector<int> Location::getLimitExcept(){return this->limitExcept;}
void Location::setLimitExcept(std::vector<int> limitExcept){this->limitExcept = limitExcept;}
bool Location::isAutoindex(){return this->autoindex;}
void Location::setAutoindex(bool autoindex){this->autoindex = autoindex;}
long Location::getMaxBody(){return this->maxBody;}
void Location::setMaxBody(long maxBody){this->maxBody = maxBody;}
std::string Location::getCgi(){return this->cgi;}
void Location::setCgi(std::string cgi){this->cgi = cgi;}
std::string Location::getCgi_index(){return this->cgi_index;}
void Location::setCgi_index(std::string cgi_index){this ->cgi_index = cgi_index;}
std::string Location::getLocationPath(){return this->locationPath;}
void Location::setLocationPath(std::string locationPath){this->locationPath = locationPath;};

