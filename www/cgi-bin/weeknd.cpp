/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   weeknd.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/30 20:06:26 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/30 20:49:09 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

int		main(void)
{
	std::cout << "Content-type: text/html\n\n";

	std::cout << "<html>\n<head>\n<title>The Weeknd - \"The Hills\"</title>\n</head>\n";
	std::cout << "<body>\n\n";
	std::cout << "<h1>The Weeknd - \"The Hills\"</h1>\n";


	std::cout << "<iframe width=\"560\" height=\"315\" src=\"https://www.youtube.com/embed/yzTuBuRdAyA\" title=\"YouTube video player\" frameborder=\"0\" allow=\"accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture\" allowfullscreen></iframe>";
	std::cout << "</body>\n</html>\n";
	return (0);
}
