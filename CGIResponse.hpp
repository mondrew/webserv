/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 15:16:21 by mondrew           #+#    #+#             */
/*   Updated: 2021/04/09 15:21:16 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_RESPONSE_HPP
# define CGI_RESPONSE_HPP

class CGIResponse {

	private:

		CGIResponst(CGIResponse const &src);
		CGIResponse		&operator=(CGIResponse const &rhs);

	public:

		CGIResponse(void);
		~CGIResponse(void);
};

#endif
