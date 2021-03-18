/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:06:28 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/18 00:44:44 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_HPP
# define SESSION_HPP

#include "Client.hpp"
#include "Server.hpp"
#include "SocketOwner.hpp"

class Session : public SocketOwner {

	private:

		// int			_socket; // in the Base class 'SocketOwner'
		Client		_client;
		char		_read_buffer[BUFFER_SIZE + 1];
		int			_read_buf_used;
		char		_write_buffer[BUFFER_SIZE];
		int			_write_buf_sent;
		Server		*_the_master;

		Session(void);

	public:

		Session(int sockfd, Server *master);
		Session(Session const &src);
		virtual ~Session(void);

		Session		&operator=(Session const &rhs);

		virtual void	handle(void);
};

#endif
