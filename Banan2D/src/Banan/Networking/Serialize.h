#pragma once

// Linux only
#include <endian.h>

// Inspired by https://stackoverflow.com/a/48643036/14181972

namespace Banan::Networking
{

	template<typename T> struct bits_t { T t; };
	template<typename T> bits_t<T&> bits(T& t) { return bits_t<T&>{t}; }
	template<typename T> bits_t<const T&> bits(const T& t) { return bits_t<const T&>{t}; }


	
	template<typename S>
	S& operator<<(S& s, bits_t<char&> b)
	{
		s.write(&b.t, sizeof(char));
		return s;
	}
	template<typename S>
	S& operator<<(S& s, bits_t<const char&> b)
	{
		s.write(&b.t, sizeof(char));
		return s;
	}
	template<typename S>
	S& operator>>(S& s, bits_t<char&> b)
	{
		s.read(&b.t, sizeof(char));
		return s;
	}


// # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #
// #  unsigned integral write
// #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # #

	template<typename S>
	S& operator<<(S& s, bits_t<uint8_t&> b)
	{
		s.write((char*)&b.t, sizeof(uint8_t));
		return s;
	}
	template<typename S>
	S& operator<<(S& s, bits_t<const uint8_t&> b)
	{
		s.write((char*)&b.t, sizeof(uint8_t));
		return s;
	}
	template<typename S>
	S& operator<<(S& s, bits_t<uint16_t&> b)
	{
		uint16_t tmp = htobe16(b.t);
		s.write((char*)&tmp, sizeof(tmp));;
		return s;
	}
	template<typename S>
	S& operator<<(S& s, bits_t<const uint16_t&> b)
	{
		uint16_t tmp = htobe16(b.t);
		s.write((char*)&tmp, sizeof(tmp));;
		return s;
	}
	template<typename S>
	S& operator<<(S& s, bits_t<uint32_t&> b)
	{
		uint32_t tmp = htobe32(b.t);
		s.write((char*)&tmp, sizeof(tmp));;
		return s;
	}
	template<typename S>
	S& operator<<(S& s, bits_t<const uint32_t&> b)
	{
		uint32_t tmp = htobe32(b.t);
		s.write((char*)&tmp, sizeof(tmp));;
		return s;
	}
	template<typename S>
	S& operator<<(S& s, bits_t<uint64_t&> b)
	{
		uint64_t tmp = htobe64(b.t);
		s.write((char*)&tmp, sizeof(tmp));
		return s;
	}
	template<typename S>
	S& operator<<(S& s, bits_t<const uint64_t&> b)
	{
		uint64_t tmp = htobe64(b.t);
		s.write((char*)&tmp, sizeof(tmp));
		return s;
	}



// # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #
// #  unsigned integral read
// #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # #

	template<typename S>
	S& operator>>(S& s, bits_t<uint8_t&> b)
	{
		s.read((char*)&b.t, sizeof(uint8_t));
		return s;
	}
	template<typename S>
	S& operator>>(S& s, bits_t<uint16_t&> b)
	{
		s.read((char*)&b.t, sizeof(uint16_t));
		b.t = be16toh(b.t);
		return s;
	}
	template<typename S>
	S& operator>>(S& s, bits_t<uint32_t&> b)
	{
		s.read((char*)&b.t, sizeof(uint32_t));
		b.t = be32toh(b.t);
		return s;
	}
	template<typename S>
	S& operator>>(S& s, bits_t<uint64_t&> b)
	{
		s.read((char*)&b.t, sizeof(uint64_t));
		b.t = be64toh(b.t);
		return s;
	}



// # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #
// #  std::string read/write
// #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # #

	template<typename S>
	S& operator<<(S& s, bits_t<std::string&> b)
	{
		s << bits((uint64_t)b.t.size());
		for (char c : b.t)
			s << bits(c);
		return s;
	}
	template<typename S>
	S& operator<<(S& s, bits_t<const std::string&> b)
	{
		s << bits((uint64_t)b.t.size());
		for (char c : b.t)
			s << bits(c);
		return s;
	}
	template<typename S>
	S& operator>>(S& s, bits_t<std::string&> b)
	{
		uint64_t size;
		s >> bits(size);
		b.t.resize(size);
		for (uint64_t i = 0; i < size; i++)
			s >> bits(b.t[i]);
		return s;
	}


}