/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventSelector.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 00:50:52 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/16 18:21:23 by gjessica         ###   ########.fr       */
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

	return;
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
	if (owner->getReadFd() > _max_fd)
		_max_fd = owner->getReadFd();
	if (owner->getWriteFd() > _max_fd)
		_max_fd = owner->getWriteFd();
	fcntl(owner->getSocket(), F_SETFL, O_NONBLOCK); // 12/05/2021
}

int EventSelector::findMaxSocket(void)
{

	int max = -1;

	for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
		 it != _socketOwnerSet.end(); it++)
	{
		if ((*it)->getSocket() > max)
			max = (*it)->getSocket();
		if ((*it)->getReadFd() > max)
			max = (*it)->getReadFd();
		if ((*it)->getWriteFd() > max)
			max = (*it)->getWriteFd();
	}
	return (max);
}

void EventSelector::remove(ASocketOwner *owner)
{

	int socket = owner->getSocket();
	int readFd = owner->getReadFd();
	int writeFd = owner->getWriteFd();
	ASocketOwner *tmp = owner;

	this->_socketOwnerSet.remove(owner);
	delete (tmp);

	if (socket == _max_fd || readFd == _max_fd || writeFd == _max_fd)
		_max_fd = findMaxSocket();
}

void EventSelector::breakLoop(void)
{
	this->_quitFlag = true;
}

// Main Loop
void EventSelector::run()
{
	_quitFlag = false;
	fd_set rds;
	fd_set wrs;
	struct timeval timeout;
	int lastMaxFd = 0;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	int counter = 0;

	int count = 0;

	while (!_quitFlag)
	{
		// Clear sets
		FD_ZERO(&rds);
		FD_ZERO(&wrs);
		// timeout 3.5 sec

		// Loop for adding in sets fds which want to read or write
		// Now I will add here all fds, but maybe it's not right
		_max_fd = findMaxSocket();
		if (lastMaxFd < _max_fd){
			lastMaxFd = _max_fd;
		//std::cout << "MAX SOCKET = " << _max_fd << std::endl;
		}
		for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
			 it != _socketOwnerSet.end(); it++)
		{
			if ((*it)->getWantToRead())
			{
				// std::cerr << "run:WantToRead on socket: " << (*it)->getSocket() << std::endl; // debug
				FD_SET((*it)->getSocket(), &rds);
			}
			if ((*it)->getWantToWrite())
			{
				// std::cerr << "run:WantToWrite: " << (*it)->getSocket() << std::endl; // debug
				FD_SET((*it)->getSocket(), &wrs);
			}
			// if ((*it)->getWantToWriteToFd())
			// {
			// 	// std::cerr << "run:WantToWriteToFd: " << (*it)->getWriteFd() << std::endl; // debug
			// 	FD_SET((*it)->getWriteFd(), &wrs);
			// }
			// if ((*it)->getWantToReadFromFd())
			// {
			// 	// std::cerr << "run:WantToReadFromFd: " << (*it)->getReadFd() << std::endl; // debug
			// 	FD_SET((*it)->getReadFd(), &rds);
			// }
		}

		// Print Sockets
		if (Util::printSockets)
		{
			std::cout << "-----------------------" << std::endl;
			std::cout << "Sockets in EventSelector" << std::endl;
			for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
				 it != _socketOwnerSet.end(); it++)
			{
				std::cout << (*it)->getSocket() << " ";
			}
			std::cout << std::endl;
			std::cout << "-----------------------" << std::endl;
			// sleep
			usleep(3);
		}

		// 'select' system call
		//std::cout << "SELECT" << std::endl; // debug
		int res = select(_max_fd + 1, &rds, &wrs, 0, &timeout);
		//std::cout << "select: ok! _max_fd: " << _max_fd << std::endl; // debug
		//std::cout << "select return: " << res << std::endl; // debug

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
			// if (counter == -1)
			// {
			// 	std::cout << "Waiting connection .  " << std::flush;
			// }
			// else if (counter == 0)
			// {
			// 	std::cout << "\rWaiting connection .  " << std::flush;
			// }
			// else if (counter == 1)
			// {
			// 	std::cout << "\rWaiting connection .. " << std::flush;
			// }
			// else if (counter == 2)
			// {
			// 	std::cout << "\rWaiting connection ..." << std::flush;
			// 	counter = -1;
			// }
			// counter++;
			//usleep(1000);
		}
		else
		{
			if (counter != -1) std::cout << std::endl;
			counter = -1;
			for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
				 it != _socketOwnerSet.end(); it++)
			{
				bool w = FD_ISSET((*it)->getSocket(), &wrs);
				bool r = FD_ISSET((*it)->getSocket(), &rds);
				if (w || r)
				{
					// if (w){
					// 	count++;
					// 	std::cout << "\rcount = " << count << std::flush;
					// }
					//fcntl((*it)->getSocket(), F_SETFL, O_NONBLOCK);
					// std::cout << "WRITE! " << (*it)->getSocket() << std::endl; // debug
					(*it)->handle();
				}

				//  else if ((*it)->getDeleteMe()){
				// 	(*it)->remove();
				// 	it= _socketOwnerSet.erase(it);
				// }
			}
			// for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
			// 	 it != _socketOwnerSet.end(); it++)
			// {

			// 	if (r)
			// 	{
			// 		//fcntl((*it)->getSocket(), F_SETFL, O_NONBLOCK);
			// 		// std::cout << "READ! " << (*it)->getSocket() << std::endl; // debug
			// 		(*it)->handle();
			// 	}

			// 	//  else if ((*it)->getDeleteMe()){
			// 	// 	(*it)->remove();
			// 	// 	it= _socketOwnerSet.erase(it);
			// 	// }
			// }


			// for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
			// 	 it != _socketOwnerSet.end(); it++)
			// {
			// 	bool rFd = FD_ISSET((*it)->getReadFd(), &rds);
			// 	if (rFd)
			// 	{
			// 		// std::cout << "READ_FD! "<< (*it)->getSocket() << std::endl; // debug
			// 		(*it)->handle();
			// 	}

			// 	//  else if ((*it)->getDeleteMe()){
			// 	// 	(*it)->remove();
			// 	// 	it= _socketOwnerSet.erase(it);
			// 	// }
			// }
			// for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
			// 	 it != _socketOwnerSet.end(); it++)
			// {


			// 	bool wFd = FD_ISSET((*it)->getWriteFd(), &wrs);

			// 	//std::cout << " | w = " << w << " | rFd = " << rFd <<  " | wfd = " << wFd << std::endl;


			// 	if (wFd)
			// 	{
			// 		//  std::cout << "WRITE_FD! "<< (*it)->getSocket() << std::endl; // debug
			// 		(*it)->handle();
			// 	}
			// }
		}
		for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
			 it != _socketOwnerSet.end(); it++)
		{
			if ((*it)->getDeleteMe())
			{
				if (Util::printCountRemoveConnection){
				count++;
				std::cout << "\rRemove " << count << std::flush;
				}
				close((*it)->getSocket());
				(*it)->remove();
				it = _socketOwnerSet.begin();
			}
		}
	}
}
