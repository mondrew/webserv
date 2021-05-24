/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventSelector.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 00:50:52 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/24 00:07:00 by mondrew          ###   ########.fr       */
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
									 _maxFd(-1)
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
	this->_maxFd = rhs._maxFd;
	return (*this);
}

void EventSelector::add(ASocketOwner *owner)
{
	this->_socketOwnerSet.push_back(owner);
	if (owner->getSocket() > _maxFd)
		_maxFd = owner->getSocket();
	if (owner->getFdCGIRequest() > _maxFd)
		_maxFd = owner->getFdCGIRequest();
	if (owner->getFdCGIResponse() > _maxFd)
		_maxFd = owner->getFdCGIResponse();
}

int EventSelector::findMaxSocket(void)
{
	int max = -1;

	for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
		 it != _socketOwnerSet.end(); it++)
	{
		if ((*it)->getSocket() > max)
			max = (*it)->getSocket();
		if ((*it)->getFdCGIRequest() > max)
			max = (*it)->getFdCGIRequest();
		if ((*it)->getFdCGIResponse() > max)
			max = (*it)->getFdCGIResponse();
	}
	return (max);
}

void EventSelector::remove(ASocketOwner *owner)
{
	int socket = owner->getSocket();
	int	fdCGIRequest = owner->getFdCGIRequest();
	int	fdCGIResponse = owner->getFdCGIResponse();

	ASocketOwner *tmp = owner;
	this->_socketOwnerSet.remove(owner);
	delete (tmp);
	if (socket == _maxFd)
		_maxFd = findMaxSocket();
	else if (fdCGIRequest == _maxFd)
		_maxFd = findMaxSocket();
	else if (fdCGIResponse == _maxFd)
		_maxFd = findMaxSocket();
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
	// int counter = 0;
	int res = 0;

	while (!_quitFlag)
	{
		// Clear sets
		FD_ZERO(&rds);
		FD_ZERO(&wrs);

		_maxFd = findMaxSocket();
		// Loop for adding in sets fds which want to read or write

		for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
			 it != _socketOwnerSet.end(); it++)
		{
			if ((*it)->getWantToRead())
			{
				// std::cout << "want to READ add to SET" << std::endl; // debug
				FD_SET((*it)->getSocket(), &rds);
			}
			if ((*it)->getWantToWrite())
			{
				// std::cout << "want to WRITE add to SET" << std::endl; // debug
				FD_SET((*it)->getSocket(), &wrs);
			}
			if ((*it)->getWantToWriteCGIRequest())
			{
				// std::cout << "want to WRITE CGI add to SET" << std::endl; // debug
				FD_SET((*it)->getFdCGIRequest(), &wrs);
			}
			if ((*it)->getWantToReadCGIResponse())
			{
				// std::cout << "want to READ CGI add to SET" << std::endl; // debug
				FD_SET((*it)->getFdCGIResponse(), &rds);
			}
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

		res = select(_maxFd + 1, &rds, &wrs, 0, &timeout);

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
			/*
			if (counter == -1)
				std::cout << "Waiting connection " << waiting[0] << std::flush;
			else
				std::cout << "\rWaiting connection " << waiting[counter] << std::flush;
			if (counter == 2)
				counter = -1;
			counter++;
			*/
		}
		else
		{
			/*
			if (counter != -1)
				std::cout << std::endl;
			counter = -1;
			*/
			for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
				 it != _socketOwnerSet.end(); it++)
			{
				// std::cout << "ES: test WRITE" << std::endl; // debug
				bool w = FD_ISSET((*it)->getSocket(), &wrs);
				if (w)
				{
					// std::cout << "ES: want to write!" << std::endl; // debug
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
				// std::cout << "ES: test READ" << std::endl; // debug
				bool r = FD_ISSET((*it)->getSocket(), &rds);
				if (r)
				{
					// std::cout << "ES: want to read!" << std::endl; // debug
					(*it)->handle(READ);
					//  _socketOwnerSet.splice(_socketOwnerSet.end(), _socketOwnerSet, it);
					//  break;
				}
			}

			for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
				 it != _socketOwnerSet.end(); it++)
			{
				// std::cout << "ES: test CGI" << std::endl; // debug
				bool w = FD_ISSET((*it)->getFdCGIRequest(), &wrs);
				bool r = FD_ISSET((*it)->getFdCGIResponse(), &rds);
				if (w)
				{
					// std::cout << "ES: want to write CGI!" << std::endl; // debug
					(*it)->handle(CGI);
				}
				if (r)
				{
					// std::cout << "ES: want to read CGI!" << std::endl; // debug
					(*it)->handle(CGI);
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
