#ifndef OSH_XORSTR_HPP
#define OSH_XORSTR_HPP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

//create a dummy XorStr function for IntelliSense
class XorStr;
XorStr _xor_(const char *x);

class XorStr
{
public:
	XorStr(int length, int key, ...)
	{
		data.reserve(length);

		int xor = key;
		int index = 0;
		int loops = length % 4 == 0 ? length / 4 : length / 4 + 1;

		va_list vl;
		va_start(vl, key);
		for (int i = 0; i < loops; ++i)
		{
			int hash = va_arg(vl, int);

			for (int j = 0; j < 4; j++)
			{
				WORD word = HIWORD(hash);

				if (j >= 2)
				{
					word = LOWORD(hash);
				}

				switch (j)
				{
				case 0:
				case 2:
					data += HIBYTE(word) ^ xor;
					break;
				case 1:
				case 3:
					data += LOBYTE(word) ^ xor;
					break;
				}
				xor += 127;
				xor %= 256;

				++index;

				if (index >= length)
				{
					break;
				}
			}  
		} 
		va_end(vl);
	}

	operator const std::string&() const
	{
		return data;
	}

	const char* c_str() const
	{
		return data.c_str();
	}

private:
	std::string data;
};

#endif