/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/23 13:32:50 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/23 20:27:31 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include <unistd.h>
#include <typeinfo>
#include <map>
#include <cmath>
#include <sstream>

std::string		toString(int val)
{
	std::ostringstream convert;
	convert << val;
	return (convert.str());
}

std::string		getDate(void)
{
	struct timeval	tv;
	struct tm		tm;
	std::string		s;
	char			buf[256];

	gettimeofday(&tv, 0);
	s = toString(tv.tv_sec);
	strptime(s.c_str(), "%s", &tm);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %T %Z", &tm);
	return (std::string(buf));
}

int		main(void)
{
	std::cout << "Content-type: text/html\n\n";

	std::cout << "<html>\n<head>\n<title>Date&Time</title>\n</head>\n";
	std::cout << "<body>\n\n";
	std::cout << "<h1>Current date & time</h1>\n";
	std::cout << "<img src=\"../pictures/mrrobot.png\" alt=\"Hello friend\">\n";
	std::cout << "<p>Hello, friend.<br>" << getDate() << "</p>\n\n";
	std::cout << "</body>\n</html>\n";
	return (0);
}
