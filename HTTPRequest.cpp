/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 15:53:09 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/20 17:58:29 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(std::string const &str) {

	// Fill all attributes from the string
	std::string		tmp = str;
	tmp = "";
}

HTTPRequest::~HTTPRequest(void) {

	return ;
}

bool			HTTPRequest::isValid(void) {

	// check if request is valid
	return (true);
}
