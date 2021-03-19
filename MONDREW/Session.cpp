/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/19 10:21:09 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"

Session::Session(int a_sockfd, Server *master) : ASocketOwner(a_sockfd),
														_the_master(master),
														_readyToResponseFlag(false) {
	return ;
}

Session::Session(Session const &src) : ASocketOwner(src) {

	*this = src;
}

Session::~Session(void) {

	return ;
}

Session		&Session::operator=(Session const &rhs) {

	this->_socket = rhs._socket;
	this->_the_master = rhs._the_master;
	this->_readyToResponseFlag = rhs._readyToResponseFlag;

	// Not right but we will never use copy constructor
	//this->_read_buffer = rhs._read_buffer;
	//this->_read_buf_used = rhs._buf_used;
	//this->_write_buffer = rhs._write_buffer;
	//this->_write_buf_sent = rhs._write_buf_sent;

	return (*this);
}

bool		Session::isReadyToResponse(void) const {

	return (this->_readyToResponseFlag);
}

void		Session::setReadyToResponseFlag(bool val) {

	this->_readyToResponseFlag = val;
}

void		Session::handle(void) {
	// When we reach EOF from 'read' system call -> means that we have received
	// whole http-request.
	// After that we
	// 	1) remove that fd from rds set
	// 	2) add that fd to the wrs set
	// 	3) here we go - we are ready to write the http-response
}
