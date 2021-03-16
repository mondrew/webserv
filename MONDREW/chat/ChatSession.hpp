/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChatSession.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 22:46:23 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/11 22:40:15 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHAT_SESSION_HPP
# define CHAT_SESSION_HPP

# include "AFdHandler.hpp"
# include "ChatServer.hpp"

enum {
	max_line_len = 1023,
	qlen_for_listen = 16
};

class ChatServer;

// Not public inheritance
class ChatSession : AFdHandler {

	private:

		// Make friend to let Server access Session fields (Server does all job)
		friend class ChatServer;

		char		_buffer[max_line_len + 1];
		int			_buf_used;
		bool		_ignoring;
		char		*_name;
		ChatServer	*_the_master;

		ChatSession(void);
		ChatSession(ChatServer *a_master, int fd);
		ChatSession(ChatSession const &src);
		~ChatSession(void);

		ChatSession		&operator=(ChatSession const &rhs);

		void			send(const char *msg);

		// handle method and it's sub-methods
		virtual void	handle(bool r, bool w);
		void			readAndIgnore(void);
		void			readAndCheck(void);
		void			checkLines(void);
		void			processLine(const char *str);

	public:
};

#endif
