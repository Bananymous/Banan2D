#pragma once

#include "Banan/Networking/Serialize.h"

#include "Banan/Core/ConsoleOutput.h"

// Message
#include <cstring>
#include <sstream>
#include <string>
#include <typeinfo>

#define BANAN_MAX_MESSAGE_SIZE 8195

namespace Banan::Networking
{

	using Socket = uint64_t;

	enum class InternetLayer
	{
		IPv4,
		IPv6
	};


	struct Message
	{
	public:
		Message() = default;

		template<typename T>
		static Message Create(const T& object)
		{
			static_assert(!std::is_pointer_v<T>, "'Banan::Networking::Message::Create(const T&)' requires T not to be a pointer");

			std::stringstream objstream;
			Banan::Networking::Serialize(objstream, object);

			uint64_t size = (uint64_t)objstream.tellp() + sizeof(uint64_t) + sizeof(uint64_t);
			uint64_t hash = typeid(T).hash_code();

			std::stringstream full;

			full << bits(size);
			full << bits(hash);
			full << objstream.rdbuf();

			Message message;
			message.m_data = full.str();

			return message;
		}

		static Message Create(void* data, uint64_t size)
		{
			Message message;
			message.m_data.resize(size);
			std::memcpy(message.m_data.data(), data, size);

			return message;
		}

		Message(const Message& other)
		{
			m_data = other.m_data;	
		}

		Message(Message&& other) noexcept
		{
			m_data = std::move(other.m_data);
			other.m_data = std::string();
		}

		Message& operator=(const Message& other)
		{
			m_data = other.m_data;
			return *this;
		}

		Message& operator=(Message&& other) noexcept
		{
			m_data = std::move(other.m_data);
			other.m_data = std::string();
			return *this;
		}

		template<typename T>
		inline bool IsType() const
		{
			return GetTypeHash() == typeid(T).hash_code();
		}

		inline uint64_t GetTypeHash() const
		{
			const char* ptr = m_data.data();
			ptr += sizeof(uint64_t);
			return *(uint64_t*)ptr;
		}

		inline uint64_t Size() const
		{
			return m_data.size();
		}

		template<typename T>
		inline void Get(T& out) const
		{
			std::istringstream iss(m_data);

			uint64_t size; iss >> bits(size);
			uint64_t hash; iss >> bits(hash);

			Banan::Networking::Deserialize(iss, out);
		}

		inline const char* GetSerialized() const
		{
			return m_data.data();
		}

		// Data must be atleast sizeof(uint64_t) bytes long
		static uint64_t GetSize(void* data)
		{
			std::istringstream iss(std::string((char*)data, sizeof(uint64_t)));
			uint64_t size;
			iss >> bits(size);
			return size;
		}

	private:
		std::string	m_data;
	};

}