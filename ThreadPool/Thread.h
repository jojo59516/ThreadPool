#ifndef _THREAD_H_
#define _THREAD_H_

#include <Windows.h>
#include <process.h>
#include <functional>
#include <memory>
#include <cassert>

namespace jojo 
{

	class thread
	{
	public:
		typedef std::function<void()> ThreadFunc;
	public:
		explicit thread(ThreadFunc&& f);
		thread(thread && th) :m_hThread(th.m_hThread), m_threadId(th.m_threadId) { th.m_hThread = 0; th.m_threadId = 0; }
		~thread() { assert(!joinable()); }

		bool joinable() const { return m_threadId != 0; }
		void join();
		void detach() { assert(joinable()); reset(); }
	private:
		thread(const thread&) {} // delete
		thread& operator=(const thread&) {} // delete

		void reset() { m_hThread = 0; m_threadId = 0; }

		static unsigned __stdcall _run(void* pArgs);
	private:
		HANDLE m_hThread;
		unsigned int m_threadId;
	};

} // namespace jojo

#endif // _THREAD_H_