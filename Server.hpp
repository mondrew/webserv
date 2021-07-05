#ifndef SERVER_HPP
# define SERVER_HPP

# include "ASocketOwner.hpp"
# include "EventSelector.hpp"
# include "Location.hpp"
# include "Session.hpp"
# include <vector>
# include <list>
# include <map>
# include <string>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <stdlib.h>
# include <arpa/inet.h>
# include <sys/wait.h>

class Session;

class Server : public ASocketOwner {

	private:

		EventSelector				*_the_selector;
		std::vector<std::string>	_serverNames;
		std::string					_host;
		int							_port;
		std::vector<Location *>		_locationSet;
		std::list<Session *>		_sessionSet;
		std::map<int, std::string>	_pagesMap;

		Server(void);

	public:

		Server(int a_socket);
		Server(Server const &src);
		virtual ~Server(void);

		Server	&operator=(Server const &rhs);

		virtual bool	isDeleteMe(void) const;
		virtual void	remove(void);

		// GETTERS
		std::vector<std::string> const		&getServerNames(void) const;
		std::string const					&getHost(void) const;
		int									getPort(void) const;
		int									getListenSocket(void) const;
		std::vector<Location *> 			&getLocationSet(void);
		std::list<Session *> const			&getSessionSet(void) const;
		std::map<int, std::string> 			&getPagesMap(void);

		// SETTERS
		void	addServerName(std::string const &server_name);
		void	setHost(std::string const &host);
		void	setPort(int port);
		void	setListenSocket(int socket);
		void	addLocation(Location *location);
		void	addSession(Session *session);
		void	setSelector(EventSelector *selector);
		void	addDefaultErrorPage(int code, std::string const &path);

		// Start server
		int				start(void);
		virtual void	handle(int action);
		void			removeSession(Session *session);
};

#endif
