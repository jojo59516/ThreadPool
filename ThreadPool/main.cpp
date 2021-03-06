// ThreadPool.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

// for detecting memory leak
#define USING_LEAK_CHECK 1
#if USING_LEAK_CHECK
#ifndef _crtdbg_map_alloc
#define _crtdbg_map_alloc
#endif
#include <crtdbg.h>
#endif

#include <vector>
#include <thread>

#include "Thread.h"
#include "ThreadPool.h"

int main()
{
	{
		auto print = [](int i) {
			Sleep(10);
			printf("std Hello, world! %d\n", i);
		};
		{
			std::vector<std::thread> threads;
			printf("======std::thread======\n");
			for (int i = 0; i < 100; ++i) {
				threads.push_back(std::thread(print, i));
				//threads.back().join();
			}
			std::for_each(threads.begin(), threads.end(), [](std::thread& th) {
				th.join();
			});
		}
		{
			std::vector<jojo::thread> threads;
			printf("======Thread======\n");
			for (int i = 0; i < 100; ++i) {
				threads.push_back(jojo::thread([=]() { print(i); }));
				//threads.back().join();
			}
			std::for_each(threads.begin(), threads.end(), [](jojo::thread& th) {
				th.join();
			});
		}
	}

	{
		jojo::ThreadPool pool(4);
		for (int i = 0; i < 100; ++i) {
			pool.enqueue([i]() {
				Sleep(10);
				printf("std Hello, world! %d\n", i);
			});
		}
		pool.join();
	}

#if USING_LEAK_CHECK
	_CrtDumpMemoryLeaks();
#endif

    return 0;
}
