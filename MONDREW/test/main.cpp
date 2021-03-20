/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 11:31:44 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/20 16:36:58 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>

#define BUFF_SIZE 5

int		main(void)
{
	std::string		str = "Hello World!";
	write(1, str.c_str(), 5);
}

/*
int		main(void)
{
	char		buf[BUFF_SIZE + 1];
	int			ret;
	std::string	str;

	while (1)
	{
		std::cout << ">>>>>start<<<<" << std::endl;
		ret = read(0, buf, BUFF_SIZE);
		std::cout << "ret: " << ret << std::endl;
		buf[ret] = '\0';
		str += buf;
		std::cout << str << std::endl;
		std::cout << "----end----" << std::endl;
	}
	return (0);
}
*/
