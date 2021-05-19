/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 15:16:21 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/19 14:06:37 by gjessica         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_RESPONSE_HPP
# define CGI_RESPONSE_HPP

# include <string>

class CGIResponse {

	private:

		std::string		_contentType;
		std::string		_location;
		std::string		_status;
		int				_statusCode;
		std::string		_statusText;
		int				_contentLength;
		std::string		_body;

		CGIResponse(CGIResponse const &src);
		CGIResponse		&operator=(CGIResponse const &rhs);

	public:

		CGIResponse(void);
		~CGIResponse(void);

		void				parseCGIResponse(std::string &str);
		void				print(void) const;

		// Getters
		std::string const	&getContentType(void) const;
		int					getContentLength(void) const;
		std::string const	&getLocation(void) const;
		std::string const	&getStatus(void) const;
		int					getStatusCode(void) const;
		std::string const	&getStatusText(void) const;
		std::string const	&getBody(void) const;

		// Setters
		void				setContentType(std::string const &contentType);
		void				setContentLength(int contentLength);
		void				setLocation(std::string const &location);
		void				setStatus(std::string const &status);
		void				setBody(std::string const &body);
};

#endif
