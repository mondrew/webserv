/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventSelector.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 00:50:52 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/12 23:50:14 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventSelector.hpp"
#include "Util.hpp"
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <iostream>

EventSelector::EventSelector(void) :
								_socketOwnerSet(std::list<ASocketOwner *>()),
								_quitFlag(false),
								_max_fd(-1) {
}

EventSelector::EventSelector(EventSelector const &src) {

	*this = src;
}

EventSelector::~EventSelector(void) {

	return ;
}

EventSelector	&EventSelector::operator=(EventSelector const &rhs) {

	this->_socketOwnerSet = rhs._socketOwnerSet;
	this->_quitFlag = rhs._quitFlag;
	this->_max_fd = rhs._max_fd;
	return (*this);
}

void			EventSelector::add(ASocketOwner *owner) {

	this->_socketOwnerSet.push_back(owner);
	if (owner->getSocket() > _max_fd)
		_max_fd = owner->getSocket();
	if (owner->getReadFd() > _max_fd)
		_max_fd = owner->getReadFd();
	if (owner->getWriteFd() > _max_fd)
		_max_fd = owner->getWriteFd();
	fcntl(owner->getSocket(), F_SETFL, O_NONBLOCK); // 12/05/2021
}

int				EventSelector::findMaxSocket(void) {

	int		max = -1;

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

void			EventSelector::remove(ASocketOwner *owner) {

	int				socket = owner->getSocket();
	int				readFd = owner->getReadFd();
	int				writeFd = owner->getWriteFd();
	ASocketOwner	*tmp = owner;

	this->_socketOwnerSet.remove(owner);
	delete (tmp);

	if (socket == _max_fd || readFd == _max_fd || writeFd == _max_fd)
		_max_fd = findMaxSocket();
}

void			EventSelector::breakLoop(void) {

	this->_quitFlag = true;
}

// Main Loop
void			EventSelector::run() {

	_quitFlag = false;

	while (!_quitFlag)
	{
		fd_set			rds;
		fd_set			wrs;
		struct timeval	timeout;

		// Clear sets
		FD_ZERO(&rds);
		FD_ZERO(&wrs);
		// timeout 3.5 sec
		timeout.tv_sec = 3000;
		timeout.tv_usec = 500000;

		// Loop for adding in sets fds which want to read or write
		// Now I will add here all fds, but maybe it's not right
		_max_fd = findMaxSocket();
		for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
											it != _socketOwnerSet.end(); it++)
		{
			if ((*it)->getWantToRead() == true)
			{
				// std::cerr << "run:WantToRead on socket: " << (*it)->getSocket() << std::endl; // debug
				FD_SET((*it)->getSocket(), &rds);
			}
			if ((*it)->getWantToWrite() == true)
			{
				// std::cerr << "run:WantToWrite: " << (*it)->getSocket() << std::endl; // debug
				FD_SET((*it)->getSocket(), &wrs);
			}
			if ((*it)->getWantToWriteToFd() == true)
			{
				// std::cerr << "run:WantToWriteToFd: " << (*it)->getWriteFd() << std::endl; // debug
				FD_SET((*it)->getWriteFd(), &wrs);
			}
			if ((*it)->getWantToReadFromFd() == true)
			{
				// std::cerr << "run:WantToReadFromFd: " << (*it)->getReadFd() << std::endl; // debug
				FD_SET((*it)->getReadFd(), &rds);
			}
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
		// std::cerr << "SELECT" << std::endl; // debug
		int		res = select(_max_fd + 1, &rds, &wrs, 0, &timeout);
		// std::cerr << "select: ok! _max_fd: " << _max_fd << std::endl; // debug
		// std::cerr << "select return: " << res << std::endl; // debug

		if (res < 0)
		{
			if (errno == EINTR)
				continue ;
			else
				break ;
		}
		else if (res == 0)
		{
			// Timeout exceeded
			std::cout << "Timeout exceeded." << std::endl;
			continue ;
		}
		else
		{
			for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
												it != _socketOwnerSet.end(); it++)
			{
				bool	r = FD_ISSET((*it)->getSocket(), &rds);
				bool	w = FD_ISSET((*it)->getSocket(), &wrs);
				bool	rFd = FD_ISSET((*it)->getReadFd(), &rds);
				bool	wFd = FD_ISSET((*it)->getWriteFd(), &wrs);
				if (r)
				{
					// std::cerr << "READ!" << std::endl; // debug
					(*it)->handle();
				}
				else if (w)
				{
					// std::cerr << "WRITE!" << std::endl; // debug
					(*it)->handle();
				}
				else if (rFd)
				{
					// std::cerr << "READ_FD!" << std::endl; // debug
					(*it)->handle();
				}
				else if (wFd)
				{
					// std::cerr << "WRITE_FD!" << std::endl; // debug
					(*it)->handle();
				}
			}

			//usleep(1000);
			for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
												it != _socketOwnerSet.end(); it++)
			{
				if ((*it)->getDeleteMe())
				{
					// std::cout << "Remove " << (*it)->getSocket() << std::endl;
					(*it)->remove();
					it = _socketOwnerSet.begin();
				}
			}
		}
	}
}
