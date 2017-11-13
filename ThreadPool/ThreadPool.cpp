#include "stdafx.h"
#include "ThreadPool.h"
#include "Thread.h"

namespace jojo {

	class lock { // for RAII
	public:
		lock(HANDLE hMutex) : m_hMutex(hMutex) {
			if (m_hMutex) { WaitForSingleObject(m_hMutex, INFINITE); }
		}
		~lock() {
			if (m_hMutex) { ReleaseMutex(m_hMutex); }
		}
	private:
		lock(const lock&) : m_hMutex(0) { assert(false); } // delete
		lock& operator=(const lock&) { assert(false); } // delete

		const HANDLE m_hMutex;
	};

	ThreadPool::ThreadPool(size_t nThreads) :
		m_hQueueMutex(CreateMutex(nullptr, false, nullptr)),
		m_hQueueEmptySemaphore(CreateSemaphore(nullptr, 0, 0x7FFFFFFF, nullptr))
	{ 
		for (size_t i = 0; i < nThreads; ++i) {
			m_workers.push_back(thread([this]() {
				while (true) {
					WaitForSingleObject(this->m_hQueueEmptySemaphore, INFINITE);
					ThreadPool::Task task;
					{
						lock _l(this->m_hQueueMutex);
						if (this->m_tasks.empty()) return;

						task = std::move(this->m_tasks.front());
						this->m_tasks.pop();
					}
					task();
				}
			}));
		}
	}

	ThreadPool::~ThreadPool() { 
		if (m_hQueueMutex) { CloseHandle(m_hQueueMutex); } 
		if (m_hQueueEmptySemaphore) { CloseHandle(m_hQueueEmptySemaphore); } 
	}

	void ThreadPool::enqueue(Task&& task) {
		{
			lock _l(m_hQueueMutex);
			m_tasks.push(std::forward<Task>(task));
		}
		ReleaseSemaphore(m_hQueueEmptySemaphore, 1, nullptr);
	}

	bool ThreadPool::joinable() const { 
		return !m_workers.empty() && m_workers.front().joinable(); 
	}

	void ThreadPool::join() {
		assert(joinable());
		ReleaseSemaphore(m_hQueueEmptySemaphore, m_workers.size(), nullptr);
		for (auto it = m_workers.begin(); it != m_workers.end(); ++it) {
			it->join();
		}
	}

	void ThreadPool::detach() {
		assert(joinable());
		ReleaseSemaphore(m_hQueueEmptySemaphore, m_workers.size(), nullptr);
		for (auto it = m_workers.begin(); it != m_workers.end(); ++it) {
			it->detach();
		}
	}
} // namespace jojo 