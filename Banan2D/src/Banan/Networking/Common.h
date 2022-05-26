#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace Banan::Networking
{

	using Socket = uint64_t;
	constexpr Socket InvalidSocket = ~Socket(0);

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
		template<typename T>
		Message(const T& object) :
			m_data(Serialize<T>(object))
		{}

		~Message()
		{
			free(m_data);
		}

	private:
		void* m_data;
	};

	// Serialize object with type T. Use malloc with custom overloading
	template<typename T>
	void* Serialize(const T& data)
	{

	}

	template<typename T>
	T& Deserialize(void* data)
	{

	}

}