/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yoda.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/23 20:52:27 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/30 22:29:26 by gjessica         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

int		main(void)
{
	std::cout << "Content-type: text/html\n\n";

	std::cout << "<html>\n<head>\n<title>Master Yoda</title>\n</head>\n";
	std::cout << "<body>\n\n";
	std::cout << "<h1>Much too learn you still have</h1>\n";
	std::cout << "<img src=\"../pictures/yoda.gif\" alt=\"Hello friend\">\n\n";
	// std::cout << "<figure>\n";
	// std::cout << "<figcaption>Across the stars theme</figcaption>\n";
	// std::cout << "<audio\ncontrols\n";
	// std::cout << "src=\"../music/across_the_stars.mp3\">";
	// std::cout << "Your browser does not support the <code>audio</code> element.";
	// std::cout << "</audio>\n</figure>";
	std::cout << "</body>\n</html>\n";
	return (0);
}
