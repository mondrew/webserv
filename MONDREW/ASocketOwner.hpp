/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocketOwner.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 00:05:41 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/18 10:28:41 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASOCKET_OWNER_HPP
# define ASOCKET_OWNER_HPP

class ASocketOwner {

	protected:

		int		_socket;

		ASocketOwner(void);

	private:

		ASocketOwner(int socket);
		ASocketOwner(ASocketOwner const &src);
		virtual ~ASocketOwner(void);

		ASocketOwner	operator=(ASocketOwner const &rhs);

		int				getSocket(void) const;
		void			setSocket(int socket);

		// Pure virtual method (implementations will be in derived classes)
		virtual void	handle(void) = 0;
};

#endif
