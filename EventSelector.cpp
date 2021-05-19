/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventSelector.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 00:50:52 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/19 13:55:38 by gjessica         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventSelector.hpp"
#include "Util.hpp"
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <iostream>

EventSelector::EventSelector(void) : _socketOwnerSet(std::list<ASocketOwner *>()),
									 _quitFlag(false),
									 _max_fd(-1)
{
}

EventSelector::EventSelector(EventSelector const &src)
{
	*this = src;
}

EventSelector::~EventSelector(void)
{
}

EventSelector &EventSelector::operator=(EventSelector const &rhs)
{
	this->_socketOwnerSet = rhs._socketOwnerSet;
	this->_quitFlag = rhs._quitFlag;
	this->_max_fd = rhs._max_fd;
	return (*this);
}

void EventSelector::add(ASocketOwner *owner)
{
	this->_socketOwnerSet.push_back(owner);
	if (owner->getSocket() > _max_fd)
		_max_fd = owner->getSocket();
}

int EventSelector::findMaxSocket(void)
{
	int max = -1;

	for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
		 it != _socketOwnerSet.end(); it++)
	{
		if ((*it)->getSocket() > max)
			max = (*it)->getSocket();
	}
	return (max);
}

void EventSelector::remove(ASocketOwner *owner)
{
	int socket = owner->getSocket();

	ASocketOwner *tmp = owner;
	this->_socketOwnerSet.remove(owner);
	delete (tmp);
	if (socket == _max_fd)
		_max_fd = findMaxSocket();
}

// Main Loop
void EventSelector::run()
{
	_quitFlag = false;
	std::string waiting[3] = {".  ", ".. ", "..."};
	fd_set rds;
	fd_set wrs;
	struct timeval timeout;
	//int lastMaxFd = 0;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	int counter = 0;
	int res = 0;

	while (!_quitFlag)
	{
		// Clear sets
		FD_ZERO(&rds);
		FD_ZERO(&wrs);

		_max_fd = findMaxSocket();
		// Loop for adding in sets fds which want to read or write

		for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
			 it != _socketOwnerSet.end(); it++)
		{
			if ((*it)->getWantToRead())
				FD_SET((*it)->getSocket(), &rds);
			if ((*it)->getWantToWrite())
				FD_SET((*it)->getSocket(), &wrs);
		}

		if (Util::printSockets)
		{
			std::cout << "-----------------------" << std::endl;
			std::cout << "Sockets in EventSelector" << std::endl;
			for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
				 it != _socketOwnerSet.end(); it++)
				std::cout << (*it)->getSocket() << " ";
			std::cout << std::endl;
			std::cout << "-----------------------" << std::endl;
		}

		res = select(_max_fd + 1, &rds, &wrs, 0, &timeout);

		if (res < 0)
		{
			std::cout << "SELECT error" << std::endl;
			if (errno == EINTR)
				continue;
			else
				break;
		}
		else if (res == 0)
		{
			if (counter == -1)
				std::cout << "Waiting connection " << waiting[0] << std::flush;
			else
				std::cout << "\rWaiting connection " << waiting[counter] << std::flush;
			if (counter == 2)
				counter = -1;
			counter++;
		}
		else
		{
			if (counter != -1)
				std::cout << std::endl;
			counter = -1;
			for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
				 it != _socketOwnerSet.end(); it++)
			{
				bool w = FD_ISSET((*it)->getSocket(), &wrs);
				if (w)
				{
					(*it)->handle(WRITE);
					//_socketOwnerSet.splice(_socketOwnerSet.end(), _socketOwnerSet, it);
					//break;
				}
			}
			/*

			INPUT	| 100000000 | | 100000000 | | 100000000 | | 100 | | 100000000 | .... X5
					|  100000	| |  100000	  | |  100000	| | 99 | |  100000	  |
					|  100000	| |  100000	  | |  100000	| |  0  | |  100000	  |
					|  100000	| |  100000	  | |  100000	| | CGI	| |  100000	  |
					...............................................................
					|  	 0		| |  	0	  | |  	  0		| |     | |     0	  |
					|   CGI		| |   CGI	  | |   CGI		| |     | |   CGI 	  |
			OUTPUT	| 100000000 | | 100000000 | | 100000000 | | 100 | | 100000000 | .... X5

			*/
			for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
				 it != _socketOwnerSet.end(); it++)
			{
				bool r = FD_ISSET((*it)->getSocket(), &rds);
				if (r){
					(*it)->handle(READ);
					//  _socketOwnerSet.splice(_socketOwnerSet.end(), _socketOwnerSet, it);
					//  break;
				}
			}

			for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
				 it != _socketOwnerSet.end(); it++)
			{
				if ((*it)->isDeleteMe() && (*it)->getSocket() > 0)
				{
					close((*it)->getSocket());
					(*it)->remove();
					it = _socketOwnerSet.begin();
				}
			}
		}
	}
}

void EventSelector::breakLoop(void)
{
	this->_quitFlag = true;
}
