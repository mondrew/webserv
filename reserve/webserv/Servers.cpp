#include "Servers.hpp"

Servers::Servers(Config config)
{
	this->config = config;
}

Servers::~Servers()
{
	return ;
}

Servers::Servers(Servers const & cp) {
	*this = src;
}

Servers &Servers::operator=(Servers const & op) {
	this->config = op.config;
	return (*this);
}

int Servers::startServers() {

	//Get servers
	std::vector<ServerConf> serversConf = this->config.getServers();

	//Get specific server
	std::vector<ServerConf>::iterator it_beg = serversConf.begin();
	std::vector<ServerConf>::iterator it_end = serversConf.end();
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
		int	opt = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(opt)) == -1)
		{
			Logger::e(ERROR_SETSOCKOPT);
			close(sock);
			it_beg++;
			continue;
		}

		/*
		*	in_addr_t inet_addr(const char *cp);
		*/
		struct sockaddr_in address_in;
		address_in.sin_family = AF_INET;
		int portListen = stoi((*it_beg).getPortListen());
		address_in.sin_port = htons(portListen);

		// inet_addr argument only ip-address format (127.0.0.1), "localhost" not worked
		address_in.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY htonl !?!?

		//std::cout << address_in.sin_family << " " << address_in.sin_port << " " << address_in.sin_addr.s_addr << std::endl;
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
		// ??? fcntl(sock, F_SETFL, O_NONBLOCK);
		std::cout << "Server " << (*it_beg).getServer_name() << " started\n";
		it_beg++;
	}
	return (0);
}

Config Servers::getConfig(){
	return this->config;
}
