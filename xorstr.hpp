#pragma once

#include <string>

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

template<typename value_type, size_t _length>
class _Basic_XorStr
{
	static constexpr auto _length_minus_one = _length - 1;

public:
	constexpr ALWAYS_INLINE _Basic_XorStr(value_type const (&str)[_length])
		: _Basic_XorStr(str, std::make_index_sequence<_length_minus_one>())
	{

	}

	inline auto c_str_ptr() const
	{
		return data;
	}

	inline auto c_str() const
	{
		decrypt();

		return data;
	}

	inline auto str() const
	{
		decrypt();

		return std::basic_string<value_type>(data, data + _length_minus_one);
	}

	inline operator std::basic_string<value_type>() const
	{
		return str();
	}

	inline void encrypt() const
	{
		if (!encrypted)
		{
			cryptLoop();
			encrypted = true;
		}
	}

	inline void decrypt() const
	{
		if (encrypted)
		{
			cryptLoop();
			encrypted = false;
		}
	}

private:
	template<size_t... indices>
	constexpr ALWAYS_INLINE _Basic_XorStr(value_type const (&str)[_length], std::index_sequence<indices...>)
		: data{ crypt(str[indices], indices)..., '\0' },
		encrypted(true)
	{

	}

	void cryptLoop() const
	{
		for (size_t t = 0; t < _length_minus_one; t++)
		{
			data[t] = crypt(data[t], t);
		}
	}

	static constexpr auto XOR_KEY = static_cast<value_type>(
		const_atoi(__TIME__[7]) +
		const_atoi(__TIME__[6]) * 10 +
		const_atoi(__TIME__[4]) * 60 +
		const_atoi(__TIME__[3]) * 600 +
		const_atoi(__TIME__[1]) * 3600 +
		const_atoi(__TIME__[0]) * 36000
		);

	static ALWAYS_INLINE constexpr auto crypt(value_type c, size_t i)
	{
		return static_cast<value_type>(c ^ (XOR_KEY + i));
	}

	mutable value_type data[_length];
	mutable bool encrypted;
};
//---------------------------------------------------------------------------
template<size_t _length>
using XorStrA = _Basic_XorStr<char, _length>;
template<size_t _length>
using XorStrW = _Basic_XorStr<wchar_t, _length>;
template<size_t _length>
using XorStrU16 = _Basic_XorStr<char16_t, _length>;
template<size_t _length>
using XorStrU32 = _Basic_XorStr<char32_t, _length>;
//---------------------------------------------------------------------------
template<typename value_type, size_t _length>
constexpr ALWAYS_INLINE auto _xor_(const value_type (&str)[_length])
{
	return _Basic_XorStr<value_type, _length>(str);
}
//---------------------------------------------------------------------------
template<typename _string_type, size_t _length, size_t _length2>
inline auto operator==(const _Basic_XorStr<_string_type, _length> &lhs, const _Basic_XorStr<_string_type, _length2> &rhs)
{
	static_assert(_length == _length2, "XorStr== different length");

	return _length == _length2 && lhs.str() == rhs.str();
}
//---------------------------------------------------------------------------
template<typename _string_type, size_t _length>
inline auto operator==(const _string_type &lhs, const _Basic_XorStr<_string_type, _length> &rhs)
{
	return lhs.size() == _length && lhs == rhs.str();
}
//---------------------------------------------------------------------------
template<typename _stream_type, typename _string_type, size_t _length>
inline auto& operator<<(_stream_type &lhs, const _Basic_XorStr<_string_type, _length> &rhs)
{
	lhs << rhs.c_str();

	return lhs;
}
//---------------------------------------------------------------------------
template<typename _string_type, size_t _length, size_t _length2>
inline auto operator+(const _Basic_XorStr<_string_type, _length> &lhs, const _Basic_XorStr<_string_type, _length2> &rhs)
{
	return lhs.str() + rhs.str();
}
//---------------------------------------------------------------------------
template<typename _string_type, size_t _length>
inline auto operator+(const _string_type &lhs, const _Basic_XorStr<_string_type, _length> &rhs)
{
	return lhs + rhs.str();
}
//---------------------------------------------------------------------------