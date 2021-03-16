#include "Server.hpp"

Server::Server()
{
	this->server_name = "localhost";
	this->portListen = "8000";
	this->default_err_page_402 = "./www/error.html";
	this->default_err_page_404 = "./www/error.html";
}

Server::Server(Server const &cp)
{
	this->server_name = cp.server_name;
	this->portListen = cp.portListen;
	this->default_err_page_402 = cp.default_err_page_402;
	this->default_err_page_404 = cp.default_err_page_404;
	this->locations = cp.locations;
}

Server::~Server()
{

}

Server &Server::operator=(Server const &cp){return *this;} // WRONG

std::string Server::getServer_name(){return this->server_name;}

void Server::setServer_name(std::string server_name){this->server_name = server_name;}

std::string Server::getPortListen(){return this->portListen;}

void Server::setPortListen(std::string portListen){this->portListen = portListen;}

std::string Server::getDefault_err_page_402(){return this->default_err_page_402;}

void Server::setDefault_err_page_402(std::string default_err_page_402){this->default_err_page_402 = default_err_page_402;}

std::string Server::getDefault_err_page_404(){return this->default_err_page_404;}

void Server::setDefault_err_page_404(std::string default_err_page_404){this->default_err_page_404 = default_err_page_404;}

void Server::addLocation(Location location)
{
	this->locations.push_back(location);
}


std::vector<Location> Server::getLocations(){return this->locations;}

// void Server::setLocations(std::list<Location> locations){this->locations = locations;}
