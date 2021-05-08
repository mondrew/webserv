/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Util.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gjessica <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/30 10:55:24 by mondrew           #+#    #+#             */
/*   Updated: 2021/05/08 11:03:27 by mondrew          ###   ########.fr       */
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
# include <cmath>
# include <sstream>
# include <list>
# include <cstdlib>

# define NOT_LIMIT -1

enum Options {
  GET		= 0x01,
  POST     	= 0x02,
  PUT   	= 0x04,
  HEAD 		= 0x08,
  UNKNOWN	= 0x00
};

class Util
{
	private:

	public:

		Util();
		~Util();
		Util(Util const &cp);
		Util &operator=(Util const &op);

		static bool		printConfig;
		static bool		printRequests;
		static bool		printResponses;
		static bool		printSockets;
		static bool		printLocations;
		static bool		printServerAccepts;
		static bool		printConnections;
		static bool		printRequestTarget;
		static bool		printRequestType;
		static bool		printCGIResponseString;
		static bool		printCGIResponse;
		static bool		printCGIRequestENVP;
		static bool		printHandleCounter;

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

		static std::string	removeLeadingWhitespaces(std::string const &str) {

			std::size_t		i = 0;

			while (i < str.length() && str[i] == ' ')
				i++;
			return (i < str.length() ? str.substr(i) : "");
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
			strftime(buf, sizeof(buf), "%a, %d %b %Y %T %Z", &tm);
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
			strftime(buf, sizeof(buf), "%a, %d %b %Y %T %Z", &tm);
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

			if (path.find("/cgi-bin/") != std::string::npos)
				return (true);
			if (dot == std::string::npos)
				return (false);
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

		static int			hexStringToInt(std::string const &hex)
		{
			int		dec = 0;
			double	power = 0;

			for (int i = hex.length() - 1; i >= 0; i--)
			{
				if (hex[i] >= 'a' && hex[i] <= 'f')
					dec += (10 + (hex[i] - 'a')) * std::pow(16.0, power);
				else if (hex[i] >= 'A' && hex[i] <= 'F')
					dec += (10 + (hex[i] - 'A')) * std::pow(16.0, power);
				else
					dec += (hex[i] - '0') * std::pow(16.0, power);
				power++;
			}
			return (dec);
		}

		static std::string	unchunkData(std::string const &data)
		{
			std::string		line;
			int				len;
			std::string		hex;
			std::size_t		start;
			std::size_t		pos = 0;
			bool			isEOF = false;

			while (!isEOF)
			{
				pos = data.find("\r\n", pos);
				if (pos != std::string::npos)
				{
					start = data.rfind("\r\n", pos - 1);
					if (start == std::string::npos)
						start = 0;
					else
						start += 2;
					hex = data.substr(start, pos - start);
					len = Util::hexStringToInt(hex);
					if (!len)
					{
						isEOF = true;
						break ;
					}
					line += data.substr(pos + 2, len);
					pos += len + 4;
				}
				else
					break ;
			}
			return (line);
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
			mimeTypes["py"] = "application/x-python-code";
			mimeTypes["cgi"] = "application/x-httpd-cgi";
			mimeTypes["wmv"] = "video/x-ms-wmv";

			std::size_t		dot = path.find_last_of(".");

			if (dot == std::string::npos)
				return ("unknown");
			return (mimeTypes[path.substr(dot + 1)]);
		}

		static std::string			itoips(long ip)
		{
			std::ostringstream	oss;

			oss << (ip & 0xFF) << "."
				<< ((ip >> 8) & 0xFF) << "."
				<< ((ip >> 16) & 0xFF) << "."
				<< ((ip >> 24) & 0xFF);
			return (oss.str());
		}

		static std::string			getServerNameFromHost(std::string const &host) {

			std::size_t		found = host.find(":");

			if (found == std::string::npos)
				return (host);
			else
				return (host.substr(0, found));
		}

		static std::string			getServerPortFromHost(std::string const &host) {

			std::size_t		found = host.find(":");

			if (found == std::string::npos)
				return ("80");
			else
				return (host.substr(found + 1));
		}

		static std::string			decodeUriEncoded(std::string str) {

			std::size_t		pos;
			std::string		hex;
			char			c;

			while ((pos = str.find("%")) != std::string::npos) {
				hex = str.substr(pos + 1, 2);
				if (hex.length() != 2)
					return (str);
				c = hexStringToInt(hex);
				str.replace(pos, 3, std::string(1, c));
			}
			return (str);
		}

		static std::string			encodeBase64(std::string data) {

			std::string		keyString = \
				"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
			std::string		encode = "";
			int				united = 0;
			int				first;
			int				second;
			int				third;
			int				counter = 1;
			char			c;

			for (std::string::iterator it = data.begin(); it < data.end(); it++) {
				if (counter == 1)
				{
					first = *it;
					first <<= 16;
					counter++;
				}
				else if (counter == 2)
				{
					second = *it;
					second <<= 8;
					counter++;
				}
				else if (counter == 3)
				{
					third = *it;
					united = first | second | third;

					// 1st 6ix
					encode += keyString[(united & 0xFC0000) >> 18];
					// 2nd 6ix
					encode += keyString[(united & 0x3F000) >> 12];
					// 3rd 6ix
					c = (united & 0xFC0) >> 6;
					if (!c && !second && !third)
						encode += '=';
					else
						encode += keyString[c];
					// 4th 6ix
					c = (united & 0x3F);
					if (!c && !third)
						encode += '=';
					else
						encode += keyString[c];
					counter = 1;
				}
			}
			while (counter != 1)
			{
				if (counter == 2)
				{
					second = 0;
					counter++;
				}
				else if (counter == 3)
				{
					third = 0;
					united = first | second | third;
					encode += keyString[united >> 18];
					encode += keyString[(united << 14) >> 26];

					c = (united & 0xFC0) >> 6;
					if (!c && !second && !third)
						encode += '=';
					else
						encode += keyString[c];

					c = (united & 0x3F);
					if (!c && !third)
						encode += '=';
					else
						encode += keyString[c];
					counter = 1;
				}
			}
			return (encode);
		}

		static std::string			decodeBase64(std::string data) {

			std::string		keyString = \
					"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
			std::string		decode = "";
			int				united;
			int				first = 0;
			int				second = 0;
			int				third = 0;
			int				fourth = 0;
			int				counter = 1;
			char			c;

			for (std::string::iterator	it = data.begin(); it < data.end(); it++) {

				if (counter == 1)
				{
					if ((std::size_t)(first = keyString.find(*it)) == std::string::npos)
						return (""); // Error
					first <<= 18;
					counter++;
				}
				else if (counter == 2)
				{
					if ((std::size_t)(second = keyString.find(*it)) == std::string::npos)
						return (""); // Error
					second <<= 12;
					counter++;
				}
				else if (counter == 3)
				{
					if (*it == '=')
						third = 0;
					else
					{
						if ((std::size_t)(third = keyString.find(*it)) == std::string::npos)
							return (""); // Error
						third <<= 6;
					}
					counter++;
				}
				else if (counter == 4)
				{
					if (*it == '=')
						fourth = 0;
					else
					{
						if ((std::size_t)(fourth = keyString.find(*it)) == std::string::npos)
							return (""); // Error
					}
					united = first | second | third | fourth;

					decode += (united >> 16) & 0xFF;
					if ((c = (united >> 8) & 0xFF) != 0)
						decode += c;
					if ((c = united & 0xFF) != 0)
						decode += c;
					counter = 1;
				}
			}
			return (decode);
		}

		static std::string			ltoips(long ip)
		{
			std::ostringstream	oss;

			oss << (ip & 0xFF) << "."
				<< ((ip >> 8) & 0xFF) << "."
				<< ((ip >> 16) & 0xFF) << "."
				<< ((ip >> 24) & 0xFF);
			return (oss.str());
		}

		static bool				isPathInfo(std::string const &str) {

			std::size_t		pos;
			std::string		tmp;

			if ((pos = str.find("cgi-bin/") != std::string::npos))
			{
				tmp = str.substr(pos + 8);
				if (tmp.find("/") != std::string::npos)
					return (true);
			}
			return (false);
		}

		static std::string		getTypeByMime(std::string const &mime) {
			if (!mime.compare("text/html")) return "html";
			if (!mime.compare("text/css")) return "css";
			if (!mime.compare("text/xml")) return "xml";
			if (!mime.compare("image/gif")) return "gif";
			if (!mime.compare("image/jpeg")) return "jpeg";
			if (!mime.compare("application/javascript")) return "js";
			if (!mime.compare("application/atom+xml")) return "atom";
			if (!mime.compare("application/rss+xml")) return "rss";

			if (!mime.compare("text/mathml")) return "mml";
			if (!mime.compare("text/rtf")) return "rtf";
			if (!mime.compare("text/plain")) return "txt";
			if (!mime.compare("text/vnd.sun.j2me.app-descriptor")) return "jad";
			if (!mime.compare("text/vnd.wap.wml")) return "wml";
			if (!mime.compare("text/x-component")) return "htc";

			if (!mime.compare("image/png")) return "png";
			if (!mime.compare("image/tiff")) return "tiff";
			if (!mime.compare("image/vnd.wap.wbmp")) return "wbmp";
			if (!mime.compare("image/x-icon")) return "ico";
			if (!mime.compare("image/x-jng")) return "jng";
			if (!mime.compare("image/x-ms-bmp")) return "bmp";
			if (!mime.compare("image/svg+xml")) return "svgz";
			if (!mime.compare("image/webp")) return "webp";

			if (!mime.compare("application/font-woff")) return "woff";
			if (!mime.compare("application/java-archive")) return "jar";
			if (!mime.compare("application/json")) return "json";
			if (!mime.compare("application/mac-binhex40")) return "hqx";
			if (!mime.compare("application/msword")) return "doc";
			if (!mime.compare("application/pdf")) return "pdf";
			if (!mime.compare("application/postscript")) return "ps";
			if (!mime.compare("application/rtf")) return "rtf";
			if (!mime.compare("application/vnd.apple.mpegurl")) return "m3u8";
			if (!mime.compare("application/vnd.ms-excel")) return "xls";
			if (!mime.compare("application/vnd.ms-fontobject")) return "eot";
			if (!mime.compare("application/vnd.ms-powerpoint")) return "ppt";
			if (!mime.compare("application/vnd.wap.wmlc")) return "wmlc";
			if (!mime.compare("application/vnd.google-earth.kml+xml")) return "kml";
			if (!mime.compare("application/vnd.google-earth.kmz")) return "kmz";
			if (!mime.compare("application/x-7z-compressed")) return "7z";
			if (!mime.compare("application/x-cocoa")) return "cco";
			if (!mime.compare("application/x-java-archive-diff")) return "jardiff";
			if (!mime.compare("application/x-java-jnlp-file")) return "jnlp";
			if (!mime.compare("application/x-makeself")) return "run";
			if (!mime.compare("application/x-perl")) return "pl";
			if (!mime.compare("application/x-pilot")) return "prc";
			if (!mime.compare("application/x-rar-compressed")) return "rar";
			if (!mime.compare("application/x-redhat-package-manager")) return "rpm";
			if (!mime.compare("application/x-sea")) return "sea";
			if (!mime.compare("application/x-shockwave-flash")) return "swf";
			if (!mime.compare("application/x-stuffit")) return "sit";
			if (!mime.compare("application/x-tcl")) return "tcl";
			if (!mime.compare("application/x-x509-ca-cert")) return "der";
			if (!mime.compare("application/x-xpinstall")) return "xpi";
			if (!mime.compare("application/xhtml+xml")) return "xhtml";
			if (!mime.compare("application/xspf+xml")) return "xspf";
			if (!mime.compare("application/zip")) return "zip";

			if (!mime.compare("application/octet-stream")) return "bin";
			if (!mime.compare("application/octet-stream")) return "deb";
			if (!mime.compare("application/octet-stream")) return "dmg";
			if (!mime.compare("application/octet-stream")) return "iso";
			if (!mime.compare("application/octet-stream")) return "msi";
			if (!mime.compare("audio/midi")) return "midi";
			if (!mime.compare("audio/mpeg")) return "mp3";
			if (!mime.compare("audio/ogg")) return "ogg";
			if (!mime.compare("audio/x-m4a")) return "m4a";
			if (!mime.compare("audio/x-realaudio")) return "ra";

			if (!mime.compare("video/3gpp")) return "3gp";
			if (!mime.compare("video/mp2t")) return "ts";
			if (!mime.compare("video/mp4")) return "mp4";
			if (!mime.compare("video/mpeg")) return "mpeg";
			if (!mime.compare("video/quicktime")) return "mov";
			if (!mime.compare("video/webm")) return "webm";
			if (!mime.compare("video/x-flv")) return "flv";
			if (!mime.compare("video/x-m4v")) return "m4v";
			if (!mime.compare("video/x-mng")) return "mng";
			if (!mime.compare("video/x-ms-asf")) return "asx";
			if (!mime.compare("video/x-ms-wmv")) return "wmv";
			if (!mime.compare("video/x-msvideo")) return "avi";
			return "unk";
		}

		static void			freeTwoDimentionalArray(const char **array)
		{
			for (int i = 0; array[i]; i++)
				free(const_cast<void *>(static_cast<const void *>(array[i])));
			free(array);
		}

		static bool			isKey(std::string const &line, std::string const &key) {

			return (toLower(line).find(toLower(key)) == 0);
		}

		static std::string	getValue(std::string const &line, std::string const &key) {

			std::size_t		pos = toLower(line).find(toLower(key));
			std::string		ret;

			if (pos == std::string::npos)
				return ("");
			ret = line.substr(pos + key.length());
			if ((pos = ret.find(":")) == std::string::npos)
				return ("");
			ret.erase(0, pos + 1);
			ret = Util::removeLeadingWhitespaces(ret);
			return (ret);
		}

		static std::string	toLower(std::string const &line) {

			std::string		lowStr = line;

			for (std::size_t i = 0; i < lowStr.length(); i++)
			{
				if (lowStr[i] >= 'A' && lowStr[i] <= 'Z')
					lowStr[i] = lowStr[i] + ('a' - 'A');
			}
			return (lowStr);
		}
};

#endif
