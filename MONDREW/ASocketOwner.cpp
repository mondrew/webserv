/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocketOwner.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 00:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/18 10:15:19 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASocketOwner.hpp"

ASocketOwner::ASocketOwner(int a_socket) : _socket(a_socket) {

	return ;
}

ASocketOwner::ASocketOwner(ASocketOwner const &src) {

	*this = src;
}

ASocketOwner::~ASocketOwner(void) {

	if (_socket != -1)
		close(_socket);
}

ASocketOwner	&ASocketOwner::operator=(ASocketOwner const &rhs) {

	this->_socket = rhs._socket;
	return (*this);
}

int				ASocketOwner::getSocket(void) const {
	return (_socket);
}

void			ASocketOwner::setSocket(int socket) {
	this->_socket = socket;
}
