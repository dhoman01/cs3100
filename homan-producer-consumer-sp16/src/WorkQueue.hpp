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
public:
	WorkQueue():cont(true), threadpool(), tasklist(){};
	~WorkQueue(){ stop(); };
	void start(int);
	void stop();
	void post(std::function<void(void)>);
};

void WorkQueue::start(int n)
{
	auto doTask =[&](){
		try
		{
			while(cont.load())
			{
				try
				{
					auto t = tasklist.dequeue();
					if(t) t();
					if(!t) std::cout << "Empty function" << std::endl;
				}
				catch(const std::exception &exc)
				{
					std::cerr << "Inner exception: " << exc.what() << std::endl;
				}
			}
		}
		catch(const std::exception &exc)
		{
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
	std::cout << "Stopping work" << std::endl;
	cont = false;
	tasklist.abort();
	std::for_each(threadpool.begin(),threadpool.end(),[](auto& t){
		std::cout << "Joining thread" << std::endl;
		t.join();
	});

	threadpool.clear();
}

void WorkQueue::post(std::function<void(void)> f)
{
    if(!f) {
    std::cout << "f is empty" << std::endl;}
	tasklist.enqueue(f);
}

#endif
