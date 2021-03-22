/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 07:58:05 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/22 13:35:32 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "ASocketOwner.hpp"
# include "EventSelector.hpp"
# include "Location.hpp"
# include "Session.hpp"
# include <vector>
# include <list>
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
		std::string					_defaultErrorPage402;
		std::string					_defaultErrorPage404;
		std::vector<Location *>		_locationSet;
		std::list<Session *>		_sessionSet;
		//bool						_readyToResponseFlag; // false

		// Клиентов будет много и не факт, что, получив HTTP-запрос от клиента мы
		// 1) Сможем его сразу (за один раз) прочитать
		// 2) Сможем отправить ответ за 1 вызов write
		// Поэтому мы должны хранить клиентов (по сути - сессии) в списке до тех пор
		// пока мы не закончим отвечать клиенту (пока не отправим весь запрос).

		Server(void);

	public:

		Server(int a_socket);
		Server(Server const &src);
		virtual ~Server(void);

		Server	&operator=(Server const &rhs);

		virtual bool	getDeleteMe(void) const { return false; }
		virtual void	remove(void) {}

		// Getters
		std::vector<std::string> const	&getServerNames(void) const;
		std::string const				&getHost(void) const;
		int								getPort(void) const;
		int								getListenSocket(void) const;
		std::string const				&getDefaultErrorPage402(void) const;
		std::string const				&getDefaultErrorPage404(void) const;
		std::vector<Location *> const	&getLocationSet(void) const;
		std::list<Session *> const		&getSessionSet(void) const;

		// Setters
		void	addServerName(std::string const &server_name);
		void	setHost(std::string const &host);
		void	setPort(int port);
		void	setListenSocket(int socket);
		void	setDefaultErrorPage402(std::string const &path);
		void	setDefaultErrorPage404(std::string const &path);
		void	addLocation(Location *location);
		void	addSession(Session *session);
		void	setSelector(EventSelector *selector);

		// Start server
		int				start(void);
		virtual void	handle(void);

		void			removeSession(Session *session);
		//virtual bool	isReadyToResponse(void) const;
};

#endif
