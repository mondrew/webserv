/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 10:27:18 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/18 10:28:11 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <vector>
# include <list>
# include <string>
# include "Server.hpp"
# include <sys/socket.h>
# include "Logger.hpp"
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>

# define MAX_USERS 1000

class Config
{
	private:

		std::string				_path;
		EventSelector			*_the_selector;
		std::list<Server *>		_serverSet;

		Config();

	public:

		Config(std::string a_path, EventSelector *a_selector);
		~Config();
		Config(Config const &cp);
		Config & operator=(Config const &op);

		std::list<ServerConf>	const	&getServers() const;
		std::string const				&getPath(void) const;
		EventSelector					*getSelector(void) const;

		void							addServer(Server server);
		void							createConfig();
		bool							isValid(void);

		void							runServers(void);
};

#endif
