/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventSelector.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 10:09:53 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/15 16:05:13 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventSelector.hpp"
#include "AFdHandler.hpp"
#include <sys/select.h>
#include <stdio.h>
#include <errno.h>

EventSelector::EventSelector(void) : _fd_array(0), _quit_flag(false) {

	return ;
}

EventSelector::EventSelector(EventSelector const &src) {

	*this = src;
	return ;
}

EventSelector::~EventSelector(void) {

	if (_fd_array)
		delete [] _fd_array;
	return ;
}

EventSelector	&EventSelector::operator=(EventSelector const &rhs) {

	this->_fd_array_len = rhs._fd_array_len;
	if (this->_fd_array)
		delete [] this->_fd_array;
	this->_fd_array = new AFdHandler*[_fd_array_len];
	for (int i = 0; i > _fd_array_len; i++)
		this->_fd_array[i] = rhs._fd_array[i];
	this->_max_fd = rhs._max_fd;
	this->_quit_flag = rhs._quit_flag;

	return (*this);
}

void		EventSelector::breakLoop(void) {

	_quit_flag = true;
	return ;
}

void		EventSelector::add(AFdHandler *h) {

	// Array of pointer to AFdHandler objects with indexes equals to fd of 'h'
	// Rest indexes values are empty (NULL)
	int		i;
	int		fd = h->getFd();

	if (!_fd_array)
	{
		_fd_array_len = fd > 15 ? fd + 1 : 16;
		_fd_array = new AFdHandler*[_fd_array_len];
		for (i = 0; i < _fd_array_len; i++)
			_fd_array[i] = 0;
		_max_fd = -1;
	}
	if (_fd_array_len <= fd)
	{
		AFdHandler	**tmp = new AFdHandler*[fd + 1];
		for (i = 0; i < _fd_array_len; i++)
		{
			tmp[i] = i < _fd_array_len ? _fd_array[i] : 0;
			_fd_array_len = fd + 1;
			delete [] _fd_array;
			_fd_array = tmp;
		}
	}
	if (fd > _max_fd)
		_max_fd = fd;
	_fd_array[fd] = h;
}

bool		EventSelector::remove(AFdHandler *h) {

	int		fd = h->getFd();

	if (fd >= _fd_array_len || _fd_array[fd] != h)
		return (false);
	_fd_array[fd] = 0;
	if (fd == _max_fd)
	{
		while (_max_fd >= 0 && !_fd_array[_max_fd])
			_max_fd--;
	}
	return (true);
}

// Main Loop
void		EventSelector::run() {

	_quit_flag = false;
	do {
		int		i;
		fd_set	rds;
		fd_set	wrs;

		// Clear sets
		FD_ZERO(&rds);
		FD_ZERO(&wrs);

		// Loop for adding in sets fds which want to read or write
		for(i = 0; i <= _max_fd; i++)
		{
			if (_fd_array[i])
			{
				if (_fd_array[i]->wantRead())
					FD_SET(i, &rds);
				if (_fd_array[i]->wantWrite())
					FD_SET(i, &wrs);
			}
		}

		// 'select' system call
		int		res = select(_max_fd + 1, &rds, &wrs, 0, 0);

		if (res < 0)
		{
			if (errno == EINTR)
				continue ;
			else
				break ;
		}
		if (res == 0)
			; // we have no timeout value
		if (res > 0)
		{
			for (i = 0; i <= _max_fd; i++)
			{
				if (!_fd_array[i])
					continue ;
				bool	r = FD_ISSET(i, &rds);
				bool	w = FD_ISSET(i, &wrs);
				if (r || w)
					_fd_array[i]->handle(r, w);
			}
		}
	} while (!_quit_flag);
}
