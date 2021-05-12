/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocketOwner.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 00:05:41 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/10 22:17:25 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASOCKET_OWNER_HPP
# define ASOCKET_OWNER_HPP

class ASocketOwner {

	protected:

		int		_socket;
		int		_readFd;
		int		_writeFd;
		bool	_wantToRead;//
		bool	_wantToWrite;//
		bool	_wantToReadFromFd;//
		bool	_wantToWriteToFd;//

	public:

		ASocketOwner(void);
		ASocketOwner(int socket);
		ASocketOwner(ASocketOwner const &src);
		virtual ~ASocketOwner(void);

		ASocketOwner	&operator=(ASocketOwner const &rhs);

		int				getSocket(void) const;
		int				getReadFd(void) const;
		int				getWriteFd(void) const;
		bool			getWantToRead(void) const;//
		bool			getWantToWrite(void) const;//
		bool			getWantToReadFromFd(void) const;//
		bool			getWantToWriteToFd(void) const;//

		void			setSocket(int socket);
		void			setReadFd(int fd);
		void			setWriteFd(int fd);

		void			setWantToRead(bool val);//
		void			setWantToWrite(bool val);//
		void			setWantToReadFromFd(bool val);//
		void			setWantToWriteToFd(bool val);//

		// Pure virtual method (implementations will be in derived classes)
		virtual void	handle(void) = 0;
		//virtual bool	isReadyToResponse(void) const = 0;

		virtual bool	getDeleteMe(void) const = 0;
		virtual void	remove(void) = 0;
};

#endif
