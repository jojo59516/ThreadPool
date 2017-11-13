#include "stdafx.h"
#include "Thread.h"

namespace jojo {
	thread::thread(ThreadFunc && f) {
		auto* fp = new ThreadFunc(std::forward<ThreadFunc>(f));
		m_hThread = (HANDLE)_beginthreadex(nullptr, 0, _run, fp, 0, &m_threadId);
	}
	void thread::join() {
		assert(joinable());
		WaitForSingleObject(m_hThread, INFINITE);
		if (m_hThread) { CloseHandle(m_hThread); }
		reset();
	}
	unsigned thread::_run(void * pArgs) {
		std::unique_ptr<ThreadFunc> pClosure(static_cast<ThreadFunc*>(pArgs));
		if (pClosure) { (*pClosure)(); }
		return 0;
	}
} // namespace jojo

