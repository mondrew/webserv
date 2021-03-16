/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 07:58:05 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/16 09:17:57 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

class Server {

	private:

		int		_port;
		int		_sockfd;

		Server(void);

	public:

		Server(int a_ip_addr, int a_port, int a_sock) : 
							_ip_addr(a_ip_addr), _port(a_port), _sock(a_sock) {
		}
		Server(Server const &src);
		~Server(void);

		Server	&operator=(Server const &rhs);

		void	init(void);
		void	run(void);

};

#endif
