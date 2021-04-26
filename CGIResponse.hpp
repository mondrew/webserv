/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 15:16:21 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/22 22:30:04 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_RESPONSE_HPP
# define CGI_RESPONSE_HPP

# include <string>

class CGIResponse {

	private:

		std::string		_contentType;
		std::string		_body;

		CGIResponse(CGIResponse const &src);
		CGIResponse		&operator=(CGIResponse const &rhs);

	public:

		CGIResponse(void);
		~CGIResponse(void);

		void				parseCGIResponse(std::string const &str);
		std::string const	&getContentType(void) const;
		std::string const	&getBody(void) const;
		void				setBody(std::string const &body);
		void				setContentType(std::string const &contentType);
};

#endif
