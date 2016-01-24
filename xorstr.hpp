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
	constexpr ALWAYS_INLINE XorStr(char const (&str)[length + 1])
		: XorStr(str, std::make_index_sequence<length - 1>())
	{

	}

	inline auto c_str() const
	{
		decrypt();

		return data;
	}

	inline auto str() const
	{
		decrypt();

		return std::string(data, data + length);
	}

	inline operator std::string() const
	{
		return str();
	}

private:
	template<size_t length, size_t... indices>
	constexpr ALWAYS_INLINE XorStr(char const (&str)[length], std::index_sequence<indices...>)
		: data{ crypt(str[indices], indices)..., '\0' },
		  encrypted(true)
	{

	}

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
		return static_cast<char>(c ^ (XOR_KEY + i));
	}

	inline void decrypt() const
	{
		if (encrypted)
		{
			for (size_t t = 0; t < length; t++)
			{
				data[t] = crypt(data[t], t);
			}
			encrypted = false;
		}
	}

	mutable char data[length + 1];
	mutable bool encrypted;
};
//---------------------------------------------------------------------------
template<size_t length, size_t length2>
inline bool operator==(const XorStr<length> &lhs, const XorStr<length2> &rhs)
{
	return length == length2 && lhs.str() == rhs.str();
}
//---------------------------------------------------------------------------
template<size_t length>
inline bool operator==(const std::string &lhs, const XorStr<length> &rhs)
{
	return lhs.size() == length && lhs == rhs.str();
}
//---------------------------------------------------------------------------
template<size_t length>
inline std::ostream& operator<<(std::ostream &lhs, const XorStr<length> &rhs)
{
	lhs << rhs.c_str();

	return lhs;
}
//---------------------------------------------------------------------------
template<size_t length, size_t length2>
inline std::string operator+(const XorStr<length> &lhs, const XorStr<length2> &rhs)
{
	return lhs.str() + rhs.str();
}
//---------------------------------------------------------------------------
template<size_t length>
inline std::string operator+(const std::string &lhs, const XorStr<length> &rhs)
{
	return lhs + rhs.str();
}
//---------------------------------------------------------------------------
template<size_t length>
constexpr ALWAYS_INLINE auto _xor_(char const (&str)[length])
{
	return XorStr<length - 1>(str);
}
//---------------------------------------------------------------------------
