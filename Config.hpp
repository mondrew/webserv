/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 10:27:18 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/24 23:08:07 by gjessica         ###   ########.fr       */
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
# include "Location.hpp"
# include <fcntl.h>

# define MAX_USERS 128

class Config
{
	private:

		std::string				_path;
		EventSelector			*_the_selector;
		std::list<Server *>		_serverSet;
		bool					_isValid;
		std::string				_error;
		Config();
		Server *parseServer(std::ifstream &file, std::string &partStr);
		Location *parseLocation(std::ifstream &file, std::string &partStr);

	public:

		Config(std::string const &a_path, EventSelector *a_selector);
		~Config();
		Config(Config const &cp);
		Config & operator=(Config const &op);

		std::list<Server *> const	&getServerSet() const;
		std::string const			&getPath(void) const;
		EventSelector				*getSelector(void) const;
		std::string	const			&getError(void) const;

		void						setError(std::string error);
		void						addServer(Server *server);
		int							parseConfig();
		bool						isValid(void);
		void						runServers(void);
};

#endif
