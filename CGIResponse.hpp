/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 15:16:21 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/01 20:33:06 by mondrew          ###   ########.fr       */
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
		std::string		_body;

		CGIResponse(CGIResponse const &src);
		CGIResponse		&operator=(CGIResponse const &rhs);

	public:

		CGIResponse(void);
		~CGIResponse(void);

		void				parseCGIResponse(std::string const &str);

		// Getters
		std::string const	&getContentType(void) const;
		std::string const	&getLocation(void) const;
		std::string const	&getStatus(void) const;
		int					getStatusCode(void) const;
		std::string const	&getStatusText(void) const;
		std::string const	&getBody(void) const;

		// Setters
		void				setContentType(std::string const &contentType);
		void				setLocation(std::string const &location);
		void				setStatus(std::string const &status);
		void				setBody(std::string const &body);
};

#endif
