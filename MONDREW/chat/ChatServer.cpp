/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChatServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 22:10:11 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/17 20:53:42 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ChatServer.hpp"
#include "ChatSession.hpp"
#include <netinet/in.h>

ChatServer::ChatServer(EventSelector *sel, int fd) : AFdHandler(fd, true),
													_the_selector(sel),
													_first(0) {
	// Add new fd in the EventSelector object array
	_the_selector->add(this);
	return ;
}

ChatServer::~ChatServer(void) {

	// Free single linked list
	while (_first)
	{
		item	*tmp = _first;
		_first = _first->next;
		_the_selector->remove(tmp->s);
		delete tmp->s;
		delete tmp;
	}
	// Delete fd from the EventSelector object array
	_the_selector->remove(this);

	return ;
}

// Static member function that creates new instance of ChatServer
ChatServer	*ChatServer::start(EventSelector *sel, int port) {

	int					ls;
	int					opt;
	int					res;
	struct sockaddr_in	addr;

	// Socket system call
	ls = socket(AF_INET, SOCK_STREAM, 0);
	if (ls == -1)
		return (0);

	opt = 1;
	setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind system call
	res = bind(ls, (struct sockaddr *)&addr, sizeof(addr));
	if (res == -1)
		return (0);

	// Listen system call
	res = listen(ls, qlen_for_listen);
	if (res == -1)
		return (0);

	return (new ChatServer(sel, ls));
}

// Derived class handle function implementation ('cause AFdHandler was abstract)
// Server must accept new connections
void		ChatServer::handle(bool r, bool w) {

	if (!r)		// must not happen
		return ;
	int		sd;
	struct sockaddr_in	addr;
	socklen_t			len = sizeof(addr);

	sd = accept(getFd(), (struct sockaddr *)&addr, &len);
	if (sd == -1)
		return ;

	// Add new client to the single-linked list 'fisrt' (register session obj)
	item	*p = new item;
	p->next = _first;
	p->s = new ChatSession(this, sd);
	_first = p;

	// Add new fd to the EventSelector object array
	_the_selector->add(p->s);
	w = false; // Werror unused
}

void		ChatServer::removeSession(ChatSession *s) {

	// Remove fd from EventSelector object array
	_the_selector->remove(s);

	// Delete Client from the single-linked list
	item	**p;

	for (p = &_first; *p; p = &((*p)->next))
	{
		if ((*p)->s == s)
		{
			item	*tmp = *p;

			*p = tmp->next;
			delete tmp->s;
			delete tmp;
			return ;
		}
	}
}

void		ChatServer::sendAll(const char *msg, ChatSession *except) {

	item	*p;

	for (p = _first; p; p = p->next)
	{
		if (p->s != except)
			p->s->send(msg);
	}
}
