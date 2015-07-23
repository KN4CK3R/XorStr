#pragma once

#include <string>
#include <utility>

namespace
{
	constexpr int const_atoi(char c)
	{
		return c - '0';
	}
}

#ifdef _MSC_VER
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE __attribute__((always_inline))
#endif

template<size_t length>
class XorStr
{
public:
	/* https://connect.microsoft.com/VisualStudio/feedback/details/1463556/delegating-constructor-in-constexpr-ctor-wont-compile
	constexpr XorStr(char const (&str)[length + 1])
		: XorStr(str, std::make_index_sequence<length - 1>())
	{
	}*/

	template<size_t length, size_t... indices>
	constexpr ALWAYS_INLINE XorStr(char const (&str)[length], std::index_sequence<indices...>)
		: data{ crypt(str[indices], indices)..., '\0' },
		  encrypted(true)
	{

	}

	inline const char* c_str() const
	{
		if (encrypted)
		{
			for (size_t t = 0; t < length; t++)
			{
				data[t] = crypt(data[t], t);
			}
			encrypted = false;
		}
		
		return data;
	}

	inline operator std::string() const
	{
		return c_str();
	}

private:
	static constexpr auto XOR_KEY = static_cast<unsigned char>(
		const_atoi(__TIME__[7]) +
		const_atoi(__TIME__[6]) * 10 +
		const_atoi(__TIME__[4]) * 60 +
		const_atoi(__TIME__[3]) * 600 +
		const_atoi(__TIME__[1]) * 3600 +
		const_atoi(__TIME__[0]) * 36000
	);
	static ALWAYS_INLINE constexpr auto crypt(char c, size_t i)
	{
		return (char)(c ^ (XOR_KEY + i));
	}

	mutable char data[length + 1];
	mutable bool encrypted;
};
//---------------------------------------------------------------------------
template<size_t length>
constexpr ALWAYS_INLINE auto _xor_defunct(char const (&str)[length]) //causes internal compiler errors on VS2015 sometimes
{
	return XorStr<length - 1>(str, std::make_index_sequence<length - 1>());
}
//---------------------------------------------------------------------------
#define _xor_(str) XorStr<sizeof(str) - 1>(str, std::make_index_sequence<sizeof(str) - 1>())
//---------------------------------------------------------------------------
