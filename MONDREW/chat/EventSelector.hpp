/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventSelector.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/09 22:57:38 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/11 22:11:43 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_SELECTOR_HPP
# define EVENT_SELECTOR_HPP

class AFdHandler;

// Stores array of fds; run() method - is a main loop
class EventSelector {

	private:

		AFdHandler	**_fd_array; // indexes - are fds
		int			_fd_array_len;
		int			_max_fd;
		bool		_quit_flag;

	public:

		EventSelector(void);
		EventSelector(EventSelector const &src);
		~EventSelector(void);

		EventSelector	&operator=(EventSelector const &rhs);

		void			add(AFdHandler *h);
		bool			remove(AFdHandler *h);

		void			breakLoop(void);
		void			run(void);
};

#endif
