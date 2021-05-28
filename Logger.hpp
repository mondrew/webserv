/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 11:14:42 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/25 20:48:26 by mondrew          ###   ########.fr       */
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
# define TEXT_RED "[1;31m"
# define TEXT_GREEN "[1;32m"
# define TEXT_DEFAULT "[0m"

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

		void static log(std::string title, std::string body, std::string color)
		{
			std::cout << "\n\033" << color << "================== " << title << " START ================== \033[0m" << std::endl;
			if (body.size() > 400)
				std::cout << body.substr(0, 400);
			else
				std::cout << body;
			if (body.length() > 400)
				std::cout << " + [" << (body.length() - 400) << " symbols]";
			std::cout << std::endl;
			std::cout << "\033" << color << "================== " << title << " END ==================== \033[0m" << std::endl;
		}
};

#endif
