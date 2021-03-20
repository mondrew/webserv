/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:06:28 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/19 11:48:40 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_HPP
# define SESSION_HPP

#include "Server.hpp"
#include "ASocketOwner.hpp"

# define BUFFER_SIZE 1024

class Server;

class Session : public ASocketOwner {

	private:

		// int			_socket; // in the Base class 'SocketOwner'
		Server		*_the_master;
		bool		_readyToResponseFlag; // false by default, until Server get
										// whole HTTP-request from Client

		char		_read_buffer[BUFFER_SIZE + 1];
		int			_read_buf_used;
		char		_write_buffer[BUFFER_SIZE];
		int			_write_buf_sent;

		Session(void);

	public:

		Session(int sockfd, Server *master);
		Session(Session const &src);
		virtual ~Session(void);

		Session		&operator=(Session const &rhs);

		virtual bool	isReadyToResponse(void) const;
		void			setReadyToResponseFlag(bool val);
		virtual void	handle(void);
};

#endif
