#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <iostream>
# include <vector>
# include <string>
# include "Util.hpp"

class Location
{
	private:

		std::string 			_locationPath;
		std::string 			_root;
		std::string				_index;
		char					_limitExcept;
		bool 					_autoindex;
		long 					_maxBody;
		std::string 			_cgi;
		std::string 			_cgiPath;

	public:

		Location();
		~Location();
		Location(Location const &cp);

		Location				&operator=(Location const &op);

		bool					isContainedInPath(std::string const &path) const;
		void 		 			print() const;

		// Getters
		std::string const		&getRoot() const;
		std::string const		&getIndex() const;
		char const				&getLimitExcept() const;
		bool					isAutoindex() const;
		long					getMaxBody() const;
		std::string const		&getCgi() const;
		std::string const		&getCgiPath() const;
		std::string const		&getLocationPath() const;

		// Setters
		void					setRoot(std::string const &root);
		void					setIndex(std::string const &index);
		void					addLimitExcept(Options limitExcept);
		void					setAutoindex(bool autoindex);
		void					setMaxBody(long maxBody);
		void					setCgi(std::string const &cgi);
		void					setCgiPath(std::string const &cgiPath);
		void					setLocationPath(std::string const &locationPath);
};

#endif
