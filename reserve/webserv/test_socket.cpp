#include <iostream>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int create_socket();

void *get_client_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
    	return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main_test()
{
	int sock = create_socket();

	if (sock < 0)
	{
		std::cout << "Error: " << sock << std::endl;
		return (0);
	}

	std::cout << "server created\n";

	struct sockaddr client_addr;
	int client_d;
	socklen_t clilen;
	while (1)
	{
		client_d = accept(sock, &client_addr, &clilen);
		char ip[INET6_ADDRSTRLEN];
		inet_ntop(client_addr.sa_family, get_client_addr(&client_addr),ip,sizeof (ip) );
		std::cout << "server connect ip " << (std::string )ip << std::endl;
		close(client_d);
	}


	return(0);
}

int create_socket()
{
	struct addrinfo hints;
	struct addrinfo *serinfo;
	struct addrinfo *p;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int r = getaddrinfo(NULL, "8000", &hints, &serinfo);
	if (r != 0)
	{
		std::cout << "Error getaddrinfo\n";
		return -1;
	}

	p = serinfo;
	int sock;
	int yes;
	while (p)
	{
		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock != -1)
		{
			if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
			{
				std::cout << "Error setsockopt\n";
				close(sock);
				freeaddrinfo(serinfo);
				return (-2);
			}

			if (bind(sock, p->ai_addr, p->ai_addrlen) == -1)
			{
				close(sock);
			} else {
				break;
			}

		}
		p = p->ai_next;
	}
	freeaddrinfo(serinfo);
	if (p == NULL)
	{
		std::cout << "error server find address\n";
		return -4;
	}
	if (listen(sock, 1000) == -1)
	{
		std::cout << "error listen\n";
		return -4;
	}
	return sock;
}
