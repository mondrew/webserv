/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 11:16:18 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/18 11:16:20 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

Logger::Logger() {
}

Logger::Logger(Logger const &cp) {

	*this = cp;
}

Logger::~Logger() {
}

Logger & Logger::operator=(Logger const &op) {

	return (*this);
}
