#pragma once

#include "Banan/Networking/Serialize.h"

#include "Banan/Core/ConsoleOutput.h"

#include <cstring>
#include <sstream>
#include <string>
#include <typeinfo>

// Thread safe queue
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

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
	public:
		Message() : m_data(nullptr) { }

		template<typename T>
		static Message Create(const T& object)
		{
			std::stringstream ss;

			ss << bytes(object);

			uint64_t size = ss.tellg() + sizeof(uint64_t) + sizeof(uint64_t);
			uint64_t hash = typeid(T).hash_code();

			std::stringstream temp;
			temp << bytes(size);
			temp << bytes(hash);
			temp << ss.rdbuf();

			ss = std::move(temp);

			Message message;
			message.m_data = ss.str();

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

		Message(Message&& other)
		{
			m_data = std::move(other.m_data);
			other.m_data = std::string();
		}

		Message& operator=(const Message& other)
		{
			m_data = other.m_data;
			return *this;
		}

		Message& operator=(Message&& other)
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
		inline T GetObject() const
		{
			uint64_t size, hash;
			T object;

			std::stringstream ss(m_data);

			ss >> size;
			ss >> hash;
			ss >> object;

			return object;
		}

		inline const char* GetSerialized() const
		{
			return m_data.data();
		}

	private:
		std::string	m_data;
	};

}