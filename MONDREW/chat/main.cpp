/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/11 10:59:31 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/13 19:22:12 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AFdHandler.hpp"
#include "EventSelector.hpp"
#include "ChatServer.hpp"
#include "ChatSession.hpp"
#include <stdio.h>

static int	port = 7777;

int		main(void)
{
	EventSelector	*selector = new EventSelector;
	ChatServer		*serv = ChatServer::start(selector, port);
	if (!serv)
	{
		perror("server");
		return (1);
	}
	selector->run();
	return (0);
}
