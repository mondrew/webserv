/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventSelector.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 20:42:54 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/29 17:02:34 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_SELECTOR_HPP
# define EVENT_SELECTOR_HPP

# include <vector>
# include <list>
# include "ASocketOwner.hpp"

class Config;

class EventSelector {

	private:

		std::list<ASocketOwner *>	_socketOwnerSet;
		bool						_quitFlag;
		int							_maxFd;

	public:

		EventSelector(void);
		EventSelector(EventSelector const &src);
		~EventSelector(void);

		EventSelector	&operator=(EventSelector const &rhs);

		void			add(ASocketOwner *owner);
		void			remove(ASocketOwner *owner);
		int				findMaxSocket(void);
		void			run(void);
		void			breakLoop(void);
};

#endif
