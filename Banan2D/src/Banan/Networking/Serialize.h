#pragma once

#include <istream>
#include <ostream>
#include <type_traits>

// Inspired by https://stackoverflow.com/a/48643036/14181972

// TODO
// std::containers could be optimized for POD types

#define BANAN_BYTE_ORDER __ORDER_BIG_ENDIAN__

namespace Banan::Networking
{
	
	using size_type = uint64_t;

	template<typename T>
	struct bits_t { T t; };

	template<typename T>
	static inline bits_t<T&>		bits(T& t)			{ return bits_t<T&>{t}; }
	template<typename T>
	static inline bits_t<const T&>	bits(const T& t)	{ return bits_t<const T&>{t}; }

	template<typename T>
	static inline bits_t<const T&>	bits_cref(bits_t<T&> b) { return bits_t<const T&>{b.t}; }

	// Arithetic types
	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	std::ostream& operator<<(std::ostream& os, bits_t<const T&> b)
	{
#if __BYTE_ORDER__ != BANAN_BYTE_ORDER
		char* ptr = (char*)&b.t;
		for (std::size_t i = sizeof(T); i > 0; i--)
			os.put(ptr[i - 1]);
#else
		os.write((char*)&b.t, sizeof(T));
#endif
		return os;
	}
	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	std::ostream& operator<<(std::ostream& os, bits_t<T&> b)
	{
		return os << bits_cref(b);
	}
	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	std::istream& operator>>(std::istream& is, bits_t<T&> b)
	{
#if __BYTE_ORDER__ != BANAN_BYTE_ORDER
		char* ptr = (char*)&b.t;
		for (std::size_t i = sizeof(T); i > 0; i--)
			is.get(((char*)&b.t)[i - 1]);
#else
		is.read((char*)&b.t, sizeof(T));
#endif
		return is;
	}

	// std::basic_string
	template<typename CharT, typename Traits, typename Allocator>
	std::ostream& operator<<(std::ostream& os, bits_t<const std::basic_string<CharT, Traits, Allocator>&> b)
	{
		os << bits<const size_type&>(b.t.size());
		for (const CharT& c : b.t)
			os << bits(c);
		return os;
	}
	template<typename CharT, typename Traits, typename Allocator>
	std::ostream& operator<<(std::ostream& os, bits_t<std::basic_string<CharT, Traits, Allocator>&> b)
	{
		return os << bits_cref(b.t);
	}
	template<typename CharT, typename Traits, typename Allocator>
	std::istream& operator>>(std::istream& is, bits_t<std::basic_string<CharT, Traits, Allocator>&> b)
	{
		size_type size;
		is >> bits(size);
		b.t.resize(size);
		for (CharT& c : b.t)
			is >> bits(c);
		return is;
	}

	// std::vector
	template<typename T, typename Allocator>
	std::ostream& operator<<(std::ostream& os, bits_t<const std::vector<T, Allocator>&> b)
	{
		os << bits<const size_type&>(b.t.size());
		for (const T& t : b.t)
			os << bits(t);
		return os;
	}
	template<typename T, typename Allocator>
	std::ostream& operator<<(std::ostream& os, bits_t<std::vector<T, Allocator>&> b)
	{
		return os << bits_cref(b);
	}
	template<typename T, typename Allocator>
	std::istream& operator>>(std::istream& is, bits_t<std::vector<T, Allocator>&> b)
	{
		size_type size;
		is >> bits(size);
		b.t.resize(size);
		for (T& t : b.t)
			is >> bits(t);
		return is;
	}

	// std::array
	template<typename T, std::size_t N>
	std::ostream& operator<<(std::ostream& os, bits_t<const std::array<T, N>&> b)
	{
		for (const T& t : b.t)
			os << bits(t);
		return os;
	}
	template<typename T, std::size_t N>
	std::ostream& operator<<(std::ostream& os, bits_t<std::array<T, N>&> b)
	{
		return os << bits_cref(b);
	}
	template<typename T, std::size_t N>
	std::istream& operator>>(std::istream& is, bits_t<std::array<T, N>&> b)
	{
		for (T& t : b.t)
			is >> bits(t);
		return is;
	}

	// std::unordered_map
	template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
	std::ostream& operator<<(std::ostream& os, bits_t<const std::unordered_map<Key, T, Hash, KeyEqual, Allocator>&> b)
	{
		os << bits<const size_type&>(b.t.size());
		for (const auto& [key, value] : b.t)
			os << bits(key) << bits(value);
		return os;
	}
	template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
	std::ostream& operator<<(std::ostream& os, bits_t<std::unordered_map<Key, T, Hash, KeyEqual, Allocator>&> b)
	{
		return os << bits_cref(b);
	}
	template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
	std::istream& operator>>(std::istream& is, bits_t<std::unordered_map<Key, T, Hash, KeyEqual, Allocator>&> b)
	{
		size_type size;
		is >> bits(size);
		while (size--) {
			Key k;
			T v;
			is >> bits(k) >> bits(v);
			b.t.insert(std::make_pair(k, v));
		}
		return is;
	}
	

	template<typename T>
	void Serialize(std::ostream& os, const T& object)
	{
		os << bits(object);
	}

	template<typename T>
	void Deserialize(std::istream& is, T& object)
	{
		is >> bits(object);
	}

}
