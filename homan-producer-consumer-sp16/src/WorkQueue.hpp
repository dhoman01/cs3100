#ifndef WORK_QUEUE
#define WORK_QUEUE

#include <algorithm>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>
#include "ThreadSafeQueue.hpp"

class WorkQueue {
private:
	ThreadSafeQueue<std::function<void(void)>> tasklist;
	std::mutex mutex_pool;
	std::atomic<bool> cont;
	std::vector<std::thread> threadpool;
public:
	WorkQueue():cont(true), threadpool(), tasklist(){};
	~WorkQueue(){ stop(); };
	void start(int);
	void stop();
	void post(std::function<void(void)>);
};

void WorkQueue::start(int n)
{
	auto doTask = [&](){
		try
		{
			while(cont.load())
			{
				try
				{
					tasklist.dequeue()();
				}
				catch(const std::exception &exc)
				{
					std::cerr << exc.what() << std::endl;
				}
			}
		}
		catch(const std::exception &exc)
		{
			std::cerr << exc.what() <<std::endl;
		}
	};
	for(auto i = 0; i < n; ++i)
	{
		threadpool.emplace_back(std::thread(doTask));
	}
}

void WorkQueue::stop()
{
	std::cout << "Stopping work" << std::endl;
	cont = false;
	tasklist.abort();
	std::for_each(threadpool.begin(),threadpool.end(),[](auto& t){
		std::cout << "Stopping thread" << std::endl;
		t.detach();
		std::cout << "Thread detached" << std::endl;
	});

	threadpool.clear();
}

void WorkQueue::post(std::function<void(void)> f)
{
	tasklist.enqueue(f);
}

#endif
