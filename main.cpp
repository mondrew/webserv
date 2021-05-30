/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:20:58 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/30 23:22:10 by gjessica         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "EventSelector.hpp"
#include "Util.hpp"

bool	Util::printConfig = false;
bool	Util::printRequests = false;
bool	Util::printResponses = false;
bool	Util::printSockets = false;
bool	Util::printLocations = false;
bool	Util::printServerAccepts = false;
bool	Util::printConnections = false;
bool	Util::printRequestTarget = false;
bool	Util::printRequestType = false;
bool	Util::printCGIResponseString = false;
bool	Util::printCGIResponse = false;
bool	Util::printCGIRequest = false;
bool	Util::printCGIRequestENVP = false;
bool	Util::printCountRemoveConnection = false;
bool	Util::printTimingLog = false;

int		main(int argc, char **argv)
{
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
