/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChatServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 22:03:07 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/18 10:24:56 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHAT_SERVER_HPP
# define CHAT_SERVER_HPP

#include "EventSelector.hpp"
#include "AFdHandler.hpp"

class ChatSession;

// 1. Handles the listening socket;
// 2. Accepts new connection (accept)
// 3. Creates object ChatSession
class ChatServer : public AFdHandler {

	private:

		EventSelector	*_the_selector;

		// Clients single-linked list
		struct item {
			ChatSession	*s;
			item		*next;
		};

		item			*_first;

		ChatServer(void);
		ChatServer(EventSelector *sel, int fd);
		ChatServer(ChatServer const &src);
		ChatServer		&operator=(ChatServer const &rhs);
		virtual void	handle(bool r, bool w);

	public:

		~ChatServer(void);

		// Prepares and creates new ChatServer instance
		static ChatServer	*start(EventSelector *sel, int port);

		void				removeSession(ChatSession *s);
		void				sendAll(const char *msg, ChatSession *except = 0);

};

#endif
