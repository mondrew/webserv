#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <sstream>


#define NOT_LIMIT -1

class Util
{
	private:

	public:
		Util();
		~Util();
		Util(Util const &cp);
		Util &operator=(Util const &op);

		std::string static toString(int val)
		{
			std::string result;
			std::ostringstream convert;
			convert << val;
			result = convert.str();
			return (result);
		}

		std::string static toString(long val)
		{
			std::string result;
			std::ostringstream convert;
			convert << val;
			result = convert.str();
			return (result);
		}

		char static getLastChar(std::string str)
		{
			int len = str.length();
			return (str[len-1]);
		}


};

#endif
