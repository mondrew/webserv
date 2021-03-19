/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventSelector.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 00:50:52 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/19 08:54:57 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventSelector.hpp"
#include <sys/select.h>
#include <cerrno>
#include <iostream>

EventSelector::EventSelector(void) :
								_socketOwnerSet(std::list<ASocketOwner *>()),
								_quitFlag(false) {
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
}

int				EventSelector::findMaxSocket(void) {

	int		max = -1;
	for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
												it != _socketOwnerSet.end(); it++)
	{
		if ((*it)->getSocket() > max)
			max = (*it)->getSocket();
	}
	return (max);
}

void			EventSelector::remove(ASocketOwner *owner) {

	int				fd = owner->getSocket();
	ASocketOwner	*tmp = owner;

	this->_socketOwnerSet.remove(owner);
	delete tmp;
	if (fd == _max_fd)
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
		// timeout 30.5 sec
		timeout.tv_sec = 30;
		timeout.tv_usec = 500000;

		// Loop for adding in sets fds which want to read or write
		// Now I will add here all fds, but maybe it's not right
		for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
												it != _socketOwnerSet.end(); it++)
		{
			FD_SET((*it)->getSocket(), &rds);
			FD_SET((*it)->getSocket(), &wrs);
		}

		// 'select' system call
		int		res = select(_max_fd + 1, &rds, &wrs, 0, &timeout);

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
				if (r || w)
					(*it)->handle();
			}
		}
	}
}
