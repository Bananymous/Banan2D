#pragma once

#include "Banan/Networking/Serialize.h"

#include "Banan/Core/ConsoleOutput.h"

// Thread safe queue
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

// Thread pool
#include <functional>
#include <future>

// Message
#include <cstring>
#include <sstream>
#include <string>
#include <typeinfo>

#define BANAN_MAX_MESSAGE_SIZE 1'000'000

namespace Banan::Networking
{

	using Socket = uint64_t;

	enum class InternetLayer
	{
		IPv4,
		IPv6
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

	struct ThreadPool
	{
	public:
		ThreadPool(uint64_t count) :
			m_running(true),
			m_threads(count),
			m_taskCount(0)
		{
			for (std::thread& t : m_threads)
				t = std::thread(&ThreadPool::ThreadFunc, this);
		}

		~ThreadPool()
		{
			m_running = false;
			m_cv_taskNew.notify_all();
			m_cv_taskDone.notify_all();

			for (std::thread& t : m_threads)
				if (t.joinable())
					t.join();
		}

		template<typename Fn, typename... Args>
		void Push(Fn&& func, Args&&... args)
		{
			std::scoped_lock _(m_mutex);

			if constexpr(sizeof...(Args) == 0)
				m_pending.push(func);
			else
				m_pending.push([func, args...]() { func(args...); });

			m_taskCount++;
			m_cv_taskNew.notify_one();
		}

		void Wait(bool clear = false)
		{
			std::unique_lock lock(m_mutex);
			if (clear)
			{
				m_pending = std::queue<std::function<void()>>();
				m_taskCount -= m_pending.size();
			}
			m_cv_taskDone.wait(lock, [&]() { return !m_running || m_taskCount == 0; });
		}

		bool IsActive() const
		{
			return m_running && m_taskCount > 0;
		}

	private:
		void ThreadFunc()
		{
			while (m_running)
			{
				std::function<void()> task;

				{
					std::unique_lock lock(m_mutex);
					m_cv_taskNew.wait(lock, [&](){ return !m_running || !m_pending.empty(); });
					if (!m_running)
						break;

					task = std::move(m_pending.front());
					m_pending.pop();
				}

				task();
				m_taskCount--;

				m_cv_taskDone.notify_all();
			}
		}

	private:
		std::atomic<bool>					m_running;
		std::condition_variable				m_cv_taskNew;
		std::condition_variable				m_cv_taskDone;


		mutable std::mutex					m_mutex;
		std::vector<std::thread>			m_threads;
		std::queue<std::function<void()>>	m_pending;

		std::atomic<uint64_t>				m_taskCount;
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