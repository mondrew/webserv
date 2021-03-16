/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AFdHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/09 22:32:22 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/16 09:08:27 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AFDHANDLER_HPP
# define AFDHANDLER_HPP

// Reacts on events that are connected with fd
class AFdHandler {

	private:

		int		_fd;
		bool	_own_fd; // shows whether it owns fd or not (close fd in dstr or not)

		AFdHandler(void);

	public:

		AFdHandler(int a_fd, bool a_own_fd);
		AFdHandler(AFdHandler const &src);
		virtual ~AFdHandler(void);

		AFdHandler	&operator=(AFdHandler const &rhs);

		// Getter
		int				getFd(void) const;

		virtual bool	wantRead(void) const;
		virtual bool	wantWrite(void) const;

		virtual void	handle(bool r, bool w) = 0; // gets info about happened event
};

#endif
