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
								_maxFd(-1) {

	return ;
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
	this->_maxFd = rhs._maxFd;
	return (*this);
}

void			EventSelector::add(ASocketOwner *owner) {

	this->_socketOwnerSet.push_back(owner);
	if (owner->getSocket() > _maxFd)
		_maxFd = owner->getSocket();
	if (owner->getFdCGIRequest() > _maxFd)
		_maxFd = owner->getFdCGIRequest();
	if (owner->getFdCGIResponse() > _maxFd)
		_maxFd = owner->getFdCGIResponse();
}

int				EventSelector::findMaxSocket(void) {

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

void			EventSelector::remove(ASocketOwner *owner) {

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
void			EventSelector::run() {

	_quitFlag = false;
	int				res = 0;
	fd_set			rds;
	fd_set			wrs;
	struct timeval	timeout;

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

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
				FD_SET((*it)->getSocket(), &rds);
			if ((*it)->getWantToWrite())
				FD_SET((*it)->getSocket(), &wrs);
			if ((*it)->getWantToWriteCGIRequest())
				FD_SET((*it)->getFdCGIRequest(), &wrs);
			if ((*it)->getWantToReadCGIResponse())
				FD_SET((*it)->getFdCGIResponse(), &rds);
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
			continue ;
		}
		else
		{
			for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
				 it != _socketOwnerSet.end(); it++)
			{
				bool w = FD_ISSET((*it)->getSocket(), &wrs);
				if (w)
					(*it)->handle(WRITE);
			}
			for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
				 it != _socketOwnerSet.end(); it++)
			{
				bool r = FD_ISSET((*it)->getSocket(), &rds);
				if (r)
					(*it)->handle(READ);
			}

			for (std::list<ASocketOwner *>::iterator it = _socketOwnerSet.begin();
				 it != _socketOwnerSet.end(); it++)
			{
				bool w = FD_ISSET((*it)->getFdCGIRequest(), &wrs);
				bool r = FD_ISSET((*it)->getFdCGIResponse(), &rds);
				if (w)
					(*it)->handle(CGI);
				if (r)
					(*it)->handle(CGI);
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

void			EventSelector::breakLoop(void) {

	this->_quitFlag = true;
}
