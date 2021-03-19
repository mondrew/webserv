/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:20:58 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/19 10:49:51 by mondrew          ###   ########.fr       */
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
		return (-1);
	}
	else if (argc > 2)
	{
		std::cout << "Error: too many arguments." << std::endl;
		return (-1);
	}

	EventSelector	*selector = new EventSelector();
	Config			config(argv[1], selector);

	// Autogenerate config. See "Config -> createConfig()"
	// Here we parse .conf file and add new Server instances to the list 'serverSet'
	config.createConfig();

	// Check if the config file if valid or not
	if (!config.isValid())
		std::cout << "Error: invalid configuration file." << std::endl;

	config.runServers();

	return (0);
}
