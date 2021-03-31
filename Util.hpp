/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Util.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/30 10:55:24 by mondrew           #+#    #+#             */
/*   Updated: 2021/03/31 08:43:11 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTIL_HPP
# define UTIL_HPP

# include <iostream>
# include <sstream>
# include <fstream>
# include <sys/time.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <time.h>
# include <unistd.h>
# include <typeinfo>
# include <map>

# define NOT_LIMIT -1

enum Options {
  GET		= 0x01,
  POST     	= 0x02,
  PUT   	= 0x04,
  HEAD 		= 0x08,
};

class Util
{
	private:

	public:

		Util();
		~Util();
		Util(Util const &cp);
		Util &operator=(Util const &op);

		static std::string	toString(int val)
		{
			std::ostringstream convert;
			convert << val;
			return (convert.str());
		}

		static std::string	toString(long val)
		{
			std::ostringstream convert;
			convert << val;
			return (convert.str());
		}

		static char			getLastChar(std::string const &str)
		{
			if (str.empty() || str.length() < 1)
				return (0);
			return (str[str.length() - 1]);
		}

		static std::string	getDate(void)
		{
			struct timeval	tv;
			struct tm		tm;
			std::string		s;
			char			buf[256];

			// 1) Fill the 'struct timeval' structure
			gettimeofday(&tv, 0);
			// 2) Convert 'long' to 'string'
			s = toString(tv.tv_sec);
			// 3) Fill 'struct tm' structure (we have only seconds scince the Epoch)
			strptime(s.c_str(), "%s", &tm);
			// 4) Now we need only to get all information we need from 'tm' to string
			strftime(buf, sizeof(buf), "%a, %d %b %Y %T GMT", &tm);
			return (std::string(buf));
		}

		static std::string	getDate(long sec)
		{
			struct tm		tm;
			std::string		s;
			char			buf[256];

			// 1) Convert 'long' to 'string'
			s = toString(sec);
			// 2) Fill 'struct tm' structure (we have only seconds scince the Epoch)
			strptime(s.c_str(), "%s", &tm);
			// 3) Now we need only to get all information we need from 'tm' to string
			strftime(buf, sizeof(buf), "%a, %d %b %Y %T GMT", &tm);
			return (std::string(buf));
		}

		static std::string	getFileLastModified(std::string filePath)
		{
			struct stat		sb;

			stat(filePath.c_str(), &sb);
			return (Util::getDate(sb.st_mtime));
		}

		static std::string	allowToString(char allow)
		{
			std::ostringstream	oss;
			bool				fst = true;

			if (allow & GET)
			{
				oss << "GET";
				fst = false;
			}
			if (allow & POST)
			{
				if (!fst)
					oss << " ";
				oss << "POST";
				fst = false;
			}
			if (allow & PUT)
			{
				if (!fst)
					oss << " ";
				oss << "PUT";
				fst = false;
			}
			if (allow & HEAD)
			{
				if (!fst)
					oss << " ";
				oss << "HEAD";
				fst = false;
			}
			return (oss.str());
		}

		static bool			isDirectory(std::string const &path)
		{
			struct stat		sb1;

			stat(path.c_str(), &sb1);
			return ((sb1.st_mode & S_IFMT) == S_IFDIR);
		}

		static bool			isAllowedToRead(std::string const &path)
		{
			struct stat		sb1;

			//std::cout << "!_!_!_!_!_!_!__!: path: " << path << std::endl; // debug
			stat(path.c_str(), &sb1);
			//std::cout << "stat: " << (sb1.st_mode & S_IROTH) << std::endl;
			return (sb1.st_mode & S_IROTH);
		}

		static std::string	fileToString(std::string const &path)
		{
			std::ifstream		ifs(path.c_str());
			std::ostringstream	oss;
			std::string			line;

			if (!ifs.is_open())
			{
				// Exception ?
				std::cout << "Error: " << path << ": no such file" << std::endl;
				return ("");
			}
			while (std::getline(ifs, line))
				oss << line << std::endl;
			if (ifs.bad())
			{
				// Exception ?
				std::cout << "Error: " << path << ": can't read the file";
				std::cout << std::endl;
			}
			ifs.close();
			return (oss.str());
		}

		static bool			isCGI(std::string const &path)
		{
			std::size_t		dot = path.find_last_of(".");
			std::string		extension;

			if (dot == std::string::npos)
				return ("unknown");
			extension = path.substr(dot + 1);
			return  (!extension.compare("php") || !extension.compare("json") || \
					!extension.compare("py") || !extension.compare("sh") || \
					!extension.compare("bash") || !extension.compare("js") || \
					!extension.compare("cgi") || !extension.compare("php-cgi"));
		}

		static std::string	removeLastPath(std::string const &path)
		{
			std::size_t		slash = path.find_last_of("/");
			std::string		restPath;

			if (slash == std::string::npos)
				return ("");
			restPath = path.substr(0, slash);
			return (restPath);
		}

		static bool			exists(std::string const &path)
		{
			struct stat		buffer;

			return (stat(path.c_str(), &buffer) == 0);
		}

		static std::string	detectContentType(std::string const &path)
		{
			std::map<std::string, std::string>	mimeTypes;

			mimeTypes["html"] = "text/html";
			mimeTypes["gif"] = "image/gif";
			mimeTypes["jpeg"] = "image/jpeg";
			mimeTypes["jpg"] = "image/jpeg";
			mimeTypes["png"] = "image/png";
			mimeTypes["tif"] = "image/tif";
			mimeTypes["tiff"] = "image/tiff";
			mimeTypes["ico"] = "image/x-icon";
			mimeTypes["bmp"] = "image/x-ms-bmp";
			mimeTypes["js"] = "application/javascript";
			mimeTypes["json"] = "application/json";
			mimeTypes["doc"] = "application/msword";
			mimeTypes["pdf"] = "application/pdf";
			mimeTypes["7z"] = "application/x-7z-compressed";
			mimeTypes["rar"] = "application/x-rar-compressed";
			mimeTypes["bin"] = "application/octet-stream";
			mimeTypes["exe"] = "application/octet-stream";
			mimeTypes["dll"] = "application/octet-stream";
			mimeTypes["iso"] = "application/octet-stream";
			mimeTypes["midi"] = "audio/midi";
			mimeTypes["mp3"] = "audio/mpeg";
			mimeTypes["m4a"] = "audio/x-m4a";
			mimeTypes["mp4"] = "video/mp4";
			mimeTypes["m4v"] = "video/m4v";
			mimeTypes["mpeg"] = "video/mpeg";
			mimeTypes["mpg"] = "video/mpeg";
			mimeTypes["mov"] = "video/quicktime";
			mimeTypes["avi"] = "video/msvideo";
			mimeTypes["wmv"] = "video/x-ms-wmv";

			std::size_t		dot = path.find_last_of(".");

			if (dot == std::string::npos)
				return ("unknown");
			return (mimeTypes[path.substr(dot + 1)]);
		}
};

#endif
