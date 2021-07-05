#ifndef ASOCKET_OWNER_HPP
# define ASOCKET_OWNER_HPP

# define READ	1
# define WRITE	2
# define CGI	3

class ASocketOwner {

	protected:

		int		_socket;
		int		_fdCGIRequest;
		int		_fdCGIResponse;

		bool	_wantToRead;
		bool	_wantToWrite;
		bool	_wantToWriteCGIRequest;
		bool	_wantToReadCGIResponse;

	public:

		ASocketOwner(void);
		ASocketOwner(int socket);
		ASocketOwner(ASocketOwner const &src);
		virtual ~ASocketOwner(void);

		ASocketOwner	&operator=(ASocketOwner const &rhs);

		// GETTERS
		int				getSocket(void) const;
		int				getFdCGIRequest(void) const;
		int				getFdCGIResponse(void) const;

		bool			getWantToRead(void) const;
		bool			getWantToWrite(void) const;
		bool			getWantToWriteCGIRequest(void) const;
		bool			getWantToReadCGIResponse(void) const;

		// SETTERS
		void			setSocket(int socket);
		void			setFdCGIRequest(int fd);
		void			setFdCGIResponse(int fd);

		void			setWantToRead(bool val);
		void			setWantToWrite(bool val);
		void			setWantToWriteCGIRequest(bool val);
		void			setWantToReadCGIResponse(bool val);

		// Pure virtual method (implementations will be in derived classes)
		virtual void	handle(int action) = 0;
		virtual bool	isDeleteMe(void) const = 0;
		virtual void	remove(void) = 0;
};

#endif
