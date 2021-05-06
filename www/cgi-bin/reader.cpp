/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reader.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/03 14:48:31 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/06 13:22:26 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>
#include <iterator>

int		main(int argc, char **argv)
{
	std::string		str;
	std::string		line;

	while (std::getline(std::cin, str))
		line += str + "\n";
	line.erase(line.size() - 1);
	std::cout << "Content-type: text/txt\n\n";
	std::cout << line;
	return (0);
}
