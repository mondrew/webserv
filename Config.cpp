#include "Config.hpp"

Config::Config()
{

}

Config::~Config()
{

}

Config::Config(Config const &cp)
{
	this->servers = cp.servers;
}

Config & Config::operator=(Config const &op)
{
	this->servers = op.servers;
	return (*this);
}

std::vector<Server> Config::getServers(){return this->servers;}

void Config::addServer(Server server){
	this->servers.push_back(server);}

Location createLocation(std::string locationPath, std::string root,
						std::string index)
{
	/*
		---------Default---------
		root			./www
		index			index.html
		limit_except	GET POST
		autoindex		on
		max_body		NOT_LIMIT
		cgi				not config
		cgi_index		not config
	*/
	Location location;
	location.setLocationPath(locationPath);
	if (!root.empty()) {location.setRoot(root);}
	if (!index.empty()) {location.setIndex(index);}

	return location;
}

Server parseServer(std::string localhost, std::string port)
{
	Server server;
	server.setServer_name(localhost);
	server.setPortListen(port);
	server.setDefault_err_page_402("./www/error.html");
	server.setDefault_err_page_404("./www/error.html");
	server.addLocation(createLocation("", "./www","")); // locationPath | root | index
	server.addLocation(createLocation("test", "./www","")); // locationPath | root | index
	return server;
}


void Config::createConfig()
{
	addServer(parseServer("localhost", "8000"));
	addServer(parseServer("localhost", "8001"));
	//You can add more servers. All values is default, except serverName and port.
}

int Config::startServers()
{
	//Get servers
	std::vector<Server> servers = this->servers;

	//Get specific server
	std::vector<Server>::iterator it_beg = servers.begin();
	std::vector<Server>::iterator it_end = servers.end();
	std::cout << "\n\n";
	while (it_beg != it_end)
	{
		/*
		* socket(int domain, int type, int protocol);
		* domain   -   	AF_INET - internetwork: UDP, TCP, etc.
		* type     -   	SOCK_STREAM - TCP stream socket | SOCK_DGRAM — UDP
		* protocol - 	If the protocol argument is zero, the default protocol
		* for this address family and type shall be used.
		*/
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0)
		{
			Logger::e(ERROR_SOCKET);
			it_beg++;
			continue;
		}
		/*
		*	setsockopt(int socket, int level, int option_name, const void *option_value,
		*				socklen_t option_len);
		*	https://www.opennet.ru/docs/RUS/socket/node5.html
		*	To manipulate options at the socket level, level is specified as SOL_SOCKET.
		*
		*/
		int option_value;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int)) == -1)
		{
			Logger::e(ERROR_SETSOCKOPT);
			close(sock);
			it_beg++;
			continue;
		}

		/*
		*	in_addr_t inet_addr(const char *cp);
		*
		*/
		struct sockaddr_in address_in;
		address_in.sin_family = AF_INET;
		address_in.sin_port = stoi((*it_beg).getPortListen());

		// inet_addr argument only ip-address format (127.0.0.1), "localhost" not worked
		address_in.sin_addr.s_addr = inet_addr("127.0.0.1");
		std::cout << address_in.sin_family << " " << address_in.sin_port << " " << address_in.sin_addr.s_addr << std::endl;
		/*
		* man 2 bind
		*  bind(int socket, const struct sockaddr *address, socklen_t address_len);
		*
		*/

		if (bind(sock, (struct sockaddr*)&address_in, sizeof(address_in)) == -1)
		{
			close(sock);
			Logger::e(ERROR_BIND);
			it_beg++;
			continue;
		}
		/*
		*	listen(int socket, int backlog);
		*/
		if (listen(sock, MAX_USERS) != 0)
		{
			close(sock);
			Logger::e(ERROR_LISTEN);
			it_beg++;
			continue;
		}
		/*
		*		fcntl(int fildes, int cmd, ...); - manipulate with open descriptor
		*		Flags as in subject
		*/
		fcntl(sock, F_SETFL, O_NONBLOCK);
		std::cout << "Server " << (*it_beg).getServer_name() << " started\n";
		it_beg++;
	}
	return (0);
}
