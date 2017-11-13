#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <vector>
#include <queue>
#include <functional>
#include <process.h>
#include <Windows.h>

namespace jojo {

	class thread;

	class ThreadPool
	{
	public:
		typedef std::function<void()> Task;
	public:
		ThreadPool(size_t nThreads = 1);
		~ThreadPool();

		void enqueue(Task&& task);

		bool joinable() const;
		void join();
		void detach();

	private:
		HANDLE m_hQueueMutex;
		HANDLE m_hQueueEmptySemaphore;
		std::queue<Task> m_tasks;

		std::vector<thread> m_workers;
	};

} // namespace jojo

#endif // _THREAD_POOL_H_
