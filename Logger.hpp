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

		Logger();
		~Logger();
		Logger(Logger const &cp);
		Logger & operator=(Logger const &op);

	public:

		int static		e(int id) {

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

		int static		e(std::string const &error) {

			std::cerr << error << std::endl;
			return (1);
		}

		int static		msg(std::string const &msg) {

			std::cout << msg << "\n";
			return (0);
		}

		void static		log(std::string const &title, \
							std::string const &body, std::string const &color) {

			std::cout << "\n\033" << color << "================== " << title;
			std::cout << " START ================== \033[0m" << std::endl;
			if (body.size() > 400)
				std::cout << body.substr(0, 400);
			else
				std::cout << body;
			if (body.length() > 400)
				std::cout << " + [" << (body.length() - 400) << " symbols]";
			std::cout << std::endl;
			std::cout << "\033" << color << "================== " << title;
			std::cout << " END ==================== \033[0m" << std::endl;
		}
};

#endif
