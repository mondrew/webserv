/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:06:28 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/27 20:55:04 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_HPP
# define SESSION_HPP

#include "Server.hpp"
#include "ASocketOwner.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

# define BUFFER_SIZE 1024

class Server;

class Session : public ASocketOwner {

	private:

		Server			*_theMaster;
		HTTPRequest		*_request;
		HTTPResponse	*_response;

		char			_buf[BUFFER_SIZE + 1];
		int				_bufLeft;
		std::string		_requestStr; // It should be 'HTTPRequest' class that owns that string
		char			_write_buffer[BUFFER_SIZE];
		std::string		_responseStr; // It should be 'HTTPResponse' class that owns that string

		bool			_deleteMe;

		Session(void);
		Session(Session const &src);
		Session		&operator=(Session const &rhs);

	public:

		Session(int sockfd, Server *master);
		virtual ~Session(void);

		void			generateResponse(void);
		void			responseToString(void);
		bool			checkRequestTarget(void);
		virtual bool	getDeleteMe(void) const;
		virtual void	remove(void);
		virtual void	handle(void);
};

#endif
