#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <sstream>


#define NOT_LIMIT -1

enum Options {
  GET		= 0x01,
  POST     	= 0x02,
  PUT   	= 0x04,
  HEAD 		= 0x08
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


};

#endif
