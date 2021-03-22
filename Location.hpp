/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 10:27:39 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/18 11:14:35 by mondrew          ###   ########.fr       */
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
		std::vector<int>	limitExcept;
		bool 				autoindex;
		long 				maxBody;
		std::string 		cgi;
		std::string 		cgi_index;

	public:

		Location();
		~Location();
		Location(Location const &cp);

		Location			&operator=(Location const &op);

		// Getters
		std::string const		&getRoot() const;
		std::string const		&getIndex() const;
		std::vector<int> const	&getLimitExcept() const;
		bool					isAutoindex() const;
		long					getMaxBody() const;
		std::string const		&getCgi() const;
		std::string const		&getCgi_index() const;
		std::string const		&getLocationPath() const;

		// Setters
		void	setRoot(std::string root);
		void	setIndex(std::string index);
		void	setLimitExcept(std::vector<int> limitExcept);
		void	setAutoindex(bool autoindex);
		void	setMaxBody(long maxBody);
		void	setCgi(std::string cgi);
		void	setCgi_index(std::string cgi_index);
		void	setLocationPath(std::string loctionPath);
};

#endif
