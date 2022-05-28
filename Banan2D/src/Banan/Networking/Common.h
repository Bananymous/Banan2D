#pragma once

#include "Banan/Core/ConsoleOutput.h"

#include <cstdlib> // malloc, free
#include <cstring> // memcpy
#include <string>
#include <typeinfo>

// Thread safe queue
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

#define BANAN_INVALID_SOCKET (~Socket(0))

namespace Banan::Networking
{

	using Socket = uint64_t;

	enum class InternetLayer
	{
		IPv4,
		IPv6
	};

	enum class TransportLayer
	{
		TCP,
		UDP
	};

	template<typename T>
	struct ThreadSafeQueue
	{
	public:
		void Push(const T& object)
		{
			std::scoped_lock lock(m_mutex);
			m_queue.push(object);
			m_empty = false;
			m_cv.notify_all();
		}

		bool Pop(T& out)
		{
			if (m_empty)
				return false;

			std::scoped_lock lock(m_mutex);
			out = m_queue.front();
			m_queue.pop();
			m_empty = m_queue.empty();
			return true;
		}

		bool WaitAndPop(T& out)
		{
			std::unique_lock lock(m_mutex);

			while (!m_kill && m_empty)
				m_cv.wait(lock);

			if (m_kill)
				return false;

			out = m_queue.front();
			m_queue.pop();
			m_empty = m_queue.empty();
			return true;
		}

		void Clear()
		{
			std::scoped_lock lock(m_mutex);
			m_queue = std::queue<T>();
			m_empty = true;
		}

		void Kill()
		{
			m_kill = true;
			m_cv.notify_all();
		}

		bool Empty() const
		{
			return m_empty;
		}

	private:
		std::atomic<bool>		m_empty = true;
		std::atomic<bool>		m_kill = false;
		std::queue<T>			m_queue;
		std::mutex				m_mutex;
		std::condition_variable	m_cv;
	};

	struct Message
	{
		using size_type = uint64_t;
		using hash_type = uint64_t;

	public:
		Message() : m_data(nullptr) {}

		// data must be allocated with std::malloc
		static Message CreateMove(void* data)
		{
			Message message;
			message.m_data = data;
			return message;
		}

		template<typename T>
		Message(T* data, size_type data_size)
		{
			size_type size = data_size + sizeof(size_type) + sizeof(hash_type);

			m_data = std::malloc(size);
			BANAN_ASSERT(m_data, "Could not allocate space for message!\n");

			uint8_t* ptr = (uint8_t*)m_data;

			*(size_type*)ptr = size;					ptr += sizeof(size_type);
			*(hash_type*)ptr = typeid(T).hash_code();	ptr += sizeof(hash_type);
			std::memcpy(ptr, data, data_size);
		}

		template<typename T, typename std::enable_if<!std::is_same<T, Message>::value, int>::type = 0>
		Message(const T& object) :
			Message(&object, sizeof(T))
		{}

		Message(const Message& other)
		{
			size_type size = *(size_type*)other.m_data;

			m_data = std::malloc(size);
			BANAN_ASSERT(m_data, "Could not allocate space for message!\n");

			std::memcpy(m_data, other.m_data, size);
		}

		Message(Message&& other)
		{
			m_data = other.m_data;
			other.m_data = nullptr;
		}

		Message& operator=(const Message& other)
		{
			size_type size = *(size_type*)other.m_data;

			m_data = std::malloc(size);
			BANAN_ASSERT(m_data, "Could not allocate space for message!\n");

			std::memcpy(m_data, other.m_data, size);

			return *this;
		}

		Message& operator=(Message&& other)
		{
			m_data = other.m_data;
			other.m_data = nullptr;
			return *this;
		}

		~Message()
		{
			std::free(m_data);
		}

		template<typename T>
		inline bool IsType() const
		{
			return GetTypeHash() == typeid(T).hash_code();
		}

		inline hash_type GetTypeHash() const
		{
			return *(hash_type*)((size_type*)m_data + 1);
		}

		inline size_type Size() const
		{
			return *(size_type*)m_data;
		}

		template<typename T>
		inline T GetObject() const
		{
			uint8_t* ptr = (uint8_t*)m_data;
			ptr += sizeof(size_type);
			ptr += sizeof(hash_type);
			return *(T*)ptr;
		}

		inline void* GetSerialized() const
		{
			return m_data;
		}

	private:
		void* m_data;
	};

	template<>
	inline std::string Message::GetObject<std::string>() const
	{
		constexpr uint64_t s = sizeof(size_type) + sizeof(hash_type);
		return std::string((char*)((uint8_t*)m_data + s), *(size_type*)m_data - s);
	}

}