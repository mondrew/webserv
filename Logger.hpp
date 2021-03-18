#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>


#define ERROR_SOCKET 1
#define ERROR_SETSOCKOPT 2
#define ERROR_BIND 3
#define ERROR_LISTEN 4

class Logger
{
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
