/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChatSession.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 22:53:50 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/11 22:33:43 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ChatSession.hpp"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

static const char welcome_msg[] = "Welcome to the chat, you are known as ";
static const char entered_msg[] = " has entered the chat";
static const char left_msg[] = " has left the chat";

ChatSession::ChatSession(ChatServer *a_master, int fd) : AFdHandler(fd, true),
														_buf_used(0),
														_ignoring(false),
														_name(0),
														_the_master(a_master) {
	send("Your name please: ");
}

ChatSession::~ChatSession(void) {

	// Delete _name
	if (_name)
		delete [] _name;
}

void		ChatSession::send(const char *msg) {

	write(getFd(), msg, strlen(msg));
}

// Client socket is ready for reading - we react on it using 'handle'
void		ChatSession::handle(bool r, bool w) {

	if (!r)		// should never happen
		return ;
	// If there are
	if (_buf_used >= (int)sizeof(_buffer))
	{
		_buf_used = 0;
		_ignoring = true;
	}
	if (_ignoring)
		readAndIgnore();
	else
		readAndCheck();
	w = false; // Werror unused
}

// Read data and ignore everything before the first '\n'
void		ChatSession::readAndIgnore(void) {

	int		rc = read(getFd(), _buffer, sizeof(_buffer));
	int		i;

	if (rc < 1)
	{
		// Ask Server to delete us (this Session)
		_the_master->removeSession(this);
		return ;
	}
	for (i = 0; i < rc; i++)
	{
		if (_buffer[i] == '\n') // stop ignoring!
		{
			int		rest = rc - i - 1;

			if (rest > 0)
				memmove(_buffer, _buffer + i + 1, rest);
			_buf_used = rest;
			_ignoring = false;

			// Check whether the whole string has came and treat it
			// and left ending without '\n'
			checkLines();
		}
	}
}

// Read and treat strings
void		ChatSession::readAndCheck(void) {

	int	rc = read(getFd(), _buffer + _buf_used, sizeof(_buffer) - _buf_used);

	if (rc < 1)
	{
		if (_name)
		{
			int		len = strlen(_name);
			char	*lmsg = new char[len + sizeof(left_msg) + 2]; // what is left_msg?

			sprintf(lmsg, "%s%s\n", _name, left_msg);
			_the_master->sendAll(lmsg, this);
			delete[] lmsg;
		}
		_the_master->removeSession(this);
		return ;
	}
	_buf_used += rc;

	// Check whether the whole string has came and treat it
	// and left ending without '\n'/ With all strings
	checkLines();
}

// Check whether the whole string has came and treat it
// and left ending without '\n'/ With all strings
void		ChatSession::checkLines(void) {

	if (_buf_used <= 0)
		return ;

	int		i;

	for (i = 0; i < _buf_used; i++)
	{
		if (_buffer[i] == '\n')
		{
			_buffer[i] = '\0';
			if (i > 0 && _buffer[i - 1] == '\r')
				_buffer[i - 1] = '\0';

			// Treat one string (before '\0')
			processLine(_buffer);

			int		rest = _buf_used - i - 1;
			memmove(_buffer, _buffer + i + 1, rest);
			_buf_used = rest;

			// Recursive call
			checkLines();
			return ;
		}
	}
}

// Treat one string (check if client already has name or not)
void		ChatSession::processLine(const char *str) {

	int		len = strlen(str);

	if (!_name)
	{
		_name = new char[len + 1];
		strcpy(_name, str);

		char	*wmsg = new char[len + sizeof(welcome_msg) + 2];
		sprintf(wmsg, "%s%s\n", welcome_msg, _name);
		send(wmsg);
		delete[] wmsg;

		char	*emsg = new char[len + sizeof(entered_msg) + 2];
		sprintf(emsg, "%s%s\n", _name, entered_msg);

		// Send to all except this session
		_the_master->sendAll(emsg, this);
		delete [] emsg;
		return ;
	}
	int		nl = strlen(_name);
	char	*msg = new char[nl + len + 5];
	sprintf(msg, "<%s> %s\n", _name, str);
	_the_master->sendAll(msg);
	delete [] msg;
}
