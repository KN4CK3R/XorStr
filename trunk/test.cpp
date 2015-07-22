#include <iostream>
#include "xorstr.hpp"

int main()
{
	std::cout << _xor_("test").c_str() << '\n';

	auto test = _xor_("test");
	std::cout << test.c_str() << '\n';

	std::string s = test;
	std::cout << s << '\n';

	std::string s2 = _xor_("test");
	std::cout << s2 << '\n';
}