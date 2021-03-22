/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocketOwner.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 00:05:41 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/22 12:48:27 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASOCKET_OWNER_HPP
# define ASOCKET_OWNER_HPP

class ASocketOwner {

	protected:

		int		_socket;
		bool	_wantToRead;//
		bool	_wantToWrite;//

	public:

		ASocketOwner(void);
		ASocketOwner(int socket);
		ASocketOwner(ASocketOwner const &src);
		virtual ~ASocketOwner(void);

		ASocketOwner	&operator=(ASocketOwner const &rhs);

		int				getSocket(void) const;
		bool			getWantToRead(void) const;//
		bool			getWantToWrite(void) const;//

		void			setSocket(int socket);
		void			setWantToRead(bool val);//
		void			setWantToWrite(bool val);//

		// Pure virtual method (implementations will be in derived classes)
		virtual void	handle(void) = 0;
		//virtual bool	isReadyToResponse(void) const = 0;

		virtual bool	getDeleteMe(void) const = 0;
		virtual void	remove(void) = 0;
};

#endif
