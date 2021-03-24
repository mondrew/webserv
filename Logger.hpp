/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 11:14:42 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/24 12:42:57 by gjessica         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <iostream>

# define ERROR_SOCKET 1
# define ERROR_SETSOCKOPT 2
# define ERROR_BIND 3
# define ERROR_LISTEN 4
# define ERROR_CONFIG_PARSE 5

class Logger {

	private:

	public:

		Logger();
		~Logger();
		Logger(Logger const &cp);
		Logger & operator=(Logger const &op);

		int static e(int id)
		{
			if (id == ERROR_SOCKET)
				e("Socket error");
			else if (id == ERROR_SETSOCKOPT)
				e("Setsockopt error");
			else if (id == ERROR_BIND)
				e("Bind error");
			else if (id == ERROR_LISTEN)
				e("Listen error");
			else if (id == ERROR_CONFIG_PARSE)
				e("Error config parse");
			e("error");
			return (1);
		}

		int static e(std::string error)
		{
			std::cerr << error << std::endl;
			return (1);
		}

		int static msg(std::string msg)
		{
			std::cout << msg << "\n";
			return (0);
		}
};

#endif
