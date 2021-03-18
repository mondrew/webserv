#include "ServerConf.hpp"

ServerConf::ServerConf()
{
	// Should it be default value of 'server_name' or not?
	this->server_names.push_back("localhost"); // MONDREW
	this->server_names.push_back("www.example.com"); // MONDREW
	this->host = "localhost"; // MONDREW
	this->portListen = "8000";
	this->default_err_page_402 = "./www/error.html";
	this->default_err_page_404 = "./www/error.html";
}

ServerConf::ServerConf(ServerConf const &cp)
{
	*this = cp;
}

ServerConf::~ServerConf()
{
	return ;
}

ServerConf &ServerConf::operator=(ServerConf const &cp)
{
	this->server_names = cp.server_names;
	this->host = cp.host;
	this->portListen = cp.portListen;
	this->default_err_page_402 = cp.default_err_page_402;
	this->default_err_page_404 = cp.default_err_page_404;
	this->locations = cp.locations;
	return *this;
}

std::vector<std::string>	ServerConf::getServer_names() {
	return this->server_names;
}

void ServerConf::setServer_names(std::vector<std::string> server_name) {
	this->server_name = server_name;
}

std::string		ServerConf::getHost() { return this->host; }

void			ServerConf::setHost(std::string a_host) { this->host = a_host; }

std::string ServerConf::getPortListen() {return this->portListen;}

void ServerConf::setPortListen(std::string portListen) {
	this->portListen = portListen;
}

std::string ServerConf::getDefault_err_page_402() {
	return this->default_err_page_402;
}

void ServerConf::setDefault_err_page_402(std::string default_err_page_402) {
	this->default_err_page_402 = default_err_page_402;
}

std::string ServerConf::getDefault_err_page_404() {
	return this->default_err_page_404;
}

void ServerConf::setDefault_err_page_404(std::string default_err_page_404) {
	this->default_err_page_404 = default_err_page_404;
}

void ServerConf::addLocation(Location location)
{
	this->locations.push_back(location);
}


std::vector<Location> ServerConf::getLocations(){return this->locations;}

// void ServerConf::setLocations(std::list<Location> locations){this->locations = locations;}

