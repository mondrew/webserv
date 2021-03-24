/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:20:58 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/24 23:08:49 by gjessica         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "EventSelector.hpp"

int		main(int argc, char **argv)
{
	// Config class contains set of Servers
	// EventSelector class - is a Main Loop for select
	if (argc < 2)
	{
		std::cout << "Error: configuration file missing." << std::endl;
		return (1);
	}
	else if (argc > 2)
	{
		std::cout << "Error: too many arguments." << std::endl;
		return (1);
	}

	EventSelector	*selector = new EventSelector();
	Config			config(argv[1], selector);
	config.parseConfig();
	if (!config.isValid())
	{
		std::cout << "Error: " << config.getError() << std::endl;
		return (1);
	}
	config.runServers();
	return (0);
}
