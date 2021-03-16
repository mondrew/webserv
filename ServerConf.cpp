#include "ServerConf.hpp"

ServerConf::ServerConf()
{
	this->server_name = "localhost";
	this->portListen = "8000";
	this->default_err_page_402 = "./www/error.html";
	this->default_err_page_404 = "./www/error.html";
}

ServerConf::ServerConf(ServerConf const &cp)
{
	this->server_name = cp.server_name;
	this->portListen = cp.portListen;
	this->default_err_page_402 = cp.default_err_page_402;
	this->default_err_page_404 = cp.default_err_page_404;
	this->locations = cp.locations;
}

ServerConf::~ServerConf()
{

}

ServerConf &ServerConf::operator=(ServerConf const &cp){return *this;} // WRONG

std::string ServerConf::getServer_name(){return this->server_name;}

void ServerConf::setServer_name(std::string server_name){this->server_name = server_name;}

std::string ServerConf::getPortListen(){return this->portListen;}

void ServerConf::setPortListen(std::string portListen){this->portListen = portListen;}

std::string ServerConf::getDefault_err_page_402(){return this->default_err_page_402;}

void ServerConf::setDefault_err_page_402(std::string default_err_page_402){this->default_err_page_402 = default_err_page_402;}

std::string ServerConf::getDefault_err_page_404(){return this->default_err_page_404;}

void ServerConf::setDefault_err_page_404(std::string default_err_page_404){this->default_err_page_404 = default_err_page_404;}

void ServerConf::addLocation(Location location)
{
	this->locations.push_back(location);
}


std::vector<Location> ServerConf::getLocations(){return this->locations;}

// void ServerConf::setLocations(std::list<Location> locations){this->locations = locations;}

