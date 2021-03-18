/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 23:10:08 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/18 00:49:05 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"

Session::Session(int a_sockfd, Server const &master) : ASocketOwner(a_sockfd),
														_the_master(master) {
	return ;
}

Session::Session(Session const &src) {

	*this = src;
}

Session::~Session {

	return ;
}

Session		operator=(Session const &rhs) : ASocketOwner(rhs) {

	this->_client = rhs._client;
	this->_the_master = rhs._the_master;
	return (*this);
}

void		Session::handler(void) {
}
