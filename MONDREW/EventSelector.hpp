/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventSelector.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 20:42:54 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/18 10:28:25 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_SELECTOR_HPP
# define EVENT_SELECTOR_HPP

# include <vector>
# include "ASocketOwner.hpp"

class EventSelector {

	private:

		std::list<ASocketOwner *>	_socketOwnerSet;
		// Сделать Server и Session наследниками ASocketOwner (общим будет fd и метод handle)
		bool						_quitFlag;
		int							_max_fd;

	public:

		EventSelector(void);
		EventSelector(EventSelector const &src);
		~EventSelector(void);

		EventSelector	&operator=(EventSelector const &rhs);

		void			add(ASocketOwner *owner);
		void			remove(ASocketOwner *owner);

		int				findMaxSocket(void);
		// Main Loop
		void			run(void);

		void			breakLoop(void);
};

#endif
