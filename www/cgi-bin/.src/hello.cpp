/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hello.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/20 14:39:04 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/23 13:11:07 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

int		main(int argc, char **argv, char **envp) {

	std::cout << "Content-type: text/txt\n\n";
	std::cout << "Hello, World!" << std::endl;

	std::cout << "argc: " << argc << std::endl;

	std::cout << "-----===== argv =====-----" << std::endl;
	while (*argv) {
		std::cout << *argv << std::endl;
		argv++;
	}
	std::cout << "-----===== envp =====-----" << std::endl;
	while (*envp) {
		std::cout << *envp << std::endl;
		envp++;
	}

	return (0);
}
