#ifndef LOCATION_HPP
#define LOCATION_HPP
#include <iostream>
#include <vector>
#include "Util.hpp"





class Location
{
	private:
		std::string 		locationPath;
		std::string 		root;
		std::string			index;
		std::vector<int>	limitExcept;
		bool 				autoindex;
		long 				maxBody;
		std::string 		cgi;
		std::string 		cgi_index;

	public:
		Location();
		~Location();
		Location(Location const &cp);
		Location &operator=(Location const &op);

		std::string			getRoot();
		void				setRoot(std::string root);
		std::string			getIndex();
		void				setIndex(std::string index);
		std::vector<int>	getLimitExcept();
		void				setLimitExcept(std::vector<int> limitExcept);
		bool				isAutoindex();
		void				setAutoindex(bool autoindex);
		long				getMaxBody();
		void				setMaxBody(long maxBody);
		std::string			getCgi();
		void				setCgi(std::string cgi);
		std::string			getCgi_index();
		void				setCgi_index(std::string cgi_index);

		std::string			getLocationPath();
		void				setLocationPath(std::string loctionPath);



};

#endif
