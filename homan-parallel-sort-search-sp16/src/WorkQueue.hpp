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
	std::atomic<bool> cont;
	std::vector<std::thread> threadpool;
	std::mutex mutex_cout;
public:
	WorkQueue():cont(true), threadpool(), tasklist(){};
	~WorkQueue(){ stop(); };
	void start(int);
	void stop();
	void post(std::function<void(void)>);
};

void WorkQueue::start(int n)
{
	auto doTask = [=](){
		try
		{
			while(cont.load())
			{
				try
				{
					auto t = tasklist.dequeue();
					t();
				}
				catch(const std::exception &exc)
				{
					std::lock_guard<std::mutex> lck(mutex_cout);
					std::cerr << "Inner exception: " << exc.what() << std::endl;
				}
			}
		}
		catch(const std::exception &exc)
		{
			std::lock_guard<std::mutex> lck(mutex_cout);
			std::cerr << "Outer exception: " << exc.what() <<std::endl;
		}
	};
	for(auto i = 0; i < n; ++i)
	{
		threadpool.emplace_back(std::thread(doTask));
	}
}

void WorkQueue::stop()
{
	cont = false;
	tasklist.abort();
	std::for_each(threadpool.begin(),threadpool.end(),[](auto& t){
		t.join();
	});

	threadpool.clear();
}

void WorkQueue::post(std::function<void(void)> f)
{
	tasklist.enqueue(f);
}

#endif
