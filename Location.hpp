/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 10:27:39 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/26 10:13:13 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <iostream>
# include <vector>
# include <string>
# include "Util.hpp"

class Location
{
	private:

		std::string 		locationPath;
		std::string 		root;
		std::string			index;
		char				limitExcept;
		bool 				autoindex;
		long 				maxBody;
		std::string 		cgi;
		std::string 		cgiPath;

	public:

		Location();
		~Location();
		Location(Location const &cp);

		Location			&operator=(Location const &op);

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
		void	setRoot(std::string root);
		void	setIndex(std::string index);
		void	addLimitExcept(Options limitExcept);
		void	setAutoindex(bool autoindex);
		void	setMaxBody(long maxBody);
		void	setCgi(std::string cgi);
		void	setCgiPath(std::string cgiPath);
		void	setLocationPath(std::string loctionPath);
};

#endif
