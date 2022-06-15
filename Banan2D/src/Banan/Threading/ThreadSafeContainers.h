#pragma once

#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <mutex>
#include <atomic>
#include <condition_variable>

namespace Banan::Threading
{

	// # # # # # # # # # # # # # # # # # # # # # # # # # # #
	// #
	// #	Thread safe queue
	// #
	// # # # # # # # # # # # # # # # # # # # # # # # # # # #

	template<typename T>
	struct TSQueue
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

		T& Front()
		{
			return m_queue.front();
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

		std::mutex& GetMutex() { return m_mutex; }

	private:
		std::atomic<bool>		m_empty = true;
		std::atomic<bool>		m_kill = false;
		std::queue<T>			m_queue;
		std::mutex				m_mutex;
		std::condition_variable	m_cv;
	};


	// # # # # # # # # # # # # # # # # # # # # # # # # # # #
	// #
	// #	Thread safe unordered map
	// #
	// # # # # # # # # # # # # # # # # # # # # # # # # # # #

	template<typename Key, typename T>
	struct TSUnorderedMap
	{
	public:
		using umap = std::unordered_map<Key, T>;

	public:
		bool HasKey(const Key& key) const
		{
			std::scoped_lock _(m_mutex);
			return m_umap.find(key) != m_umap.end();
		}

		T GetCopy(const Key& key) const
		{
			std::scoped_lock _(m_mutex);
			return m_umap.at(key);
		}

		void Insert(const Key& key, const T& value)
		{
			std::scoped_lock _(m_mutex);
			m_umap[key] = value;
		}


		std::mutex& GetMutex() { return m_mutex; }

		T& GetRef(const Key& key)
		{
			return m_umap[key];
		}

		decltype(auto) begin()			{ return m_umap.begin(); }
		decltype(auto) begin()	const	{ return m_umap.cbegin(); }
		decltype(auto) end()			{ return m_umap.end(); }
		decltype(auto) end()	const	{ return m_umap.cend(); }

	private:
		mutable std::mutex	m_mutex;
		umap				m_umap;
	};


	// # # # # # # # # # # # # # # # # # # # # # # # # # # #
	// #
	// #	Thread safe unordered set
	// #
	// # # # # # # # # # # # # # # # # # # # # # # # # # # #

	template<typename T>
	struct TSUnorderedSet
	{
	public:
		using uset = std::unordered_set<T>;

	public:
		void Insert(const T& val)
		{
			std::scoped_lock _(m_mutex);
			m_uset.insert(val);
		}

		void Erase(const T& val)
		{
			std::scoped_lock _(m_mutex);
			m_uset.erase(val);
		}

		void HasKey(const T& val)
		{
			std::scoped_lock _(m_mutex);
			return m_uset.find(val) != m_uset.end();
		}


		std::mutex& GetMutex() { return m_mutex; }

		decltype(auto) begin()			{ return m_uset.begin(); }
		decltype(auto) begin()	const	{ return m_uset.cbegin(); }
		decltype(auto) end()			{ return m_uset.end(); }
		decltype(auto) end()	const	{ return m_uset.cend(); }

	private:
		mutable std::mutex	m_mutex;
		uset				m_uset;
	};

}