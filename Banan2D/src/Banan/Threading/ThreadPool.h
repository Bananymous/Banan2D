#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>

namespace Banan::Threading
{

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

			if constexpr (sizeof...(Args) == 0)
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
					m_cv_taskNew.wait(lock, [&]() { return !m_running || !m_pending.empty(); });
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

}