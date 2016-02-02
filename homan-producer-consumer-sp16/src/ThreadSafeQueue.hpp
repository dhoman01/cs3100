#ifndef THREAD_SAFE_QUEUE
#define THREAD_SAFE_QUEUE

#include <mutex>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <exception>
#include <iostream>

template<class T>
class ThreadSafeQueue{
private:
	std::queue<T> data;
	std::mutex mutex_data;
	std::mutex mutex_wait;
	std::condition_variable empty;
	std::atomic<bool> cont;
public:
	ThreadSafeQueue():data(),mutex_data(),empty(),cont(true){};
	ThreadSafeQueue(const ThreadSafeQueue& tsq) : data(tsq.data),mutex_data(),empty(),cont(true){};
	void enqueue(T t)
	{
		std::lock_guard<std::mutex> l(mutex_data);
		data.push(t);
		empty.notify_one();
	};
	T dequeue()
	{
		while(data.empty())
		{
			if(!cont.load())
			{
				std::cout << "Thread is aborting..." << std::endl;
				throw std::runtime_error("Wait aborted");
			}
			{
				std::unique_lock<std::mutex> lck (mutex_wait);
				std::cout << "Thread waiting for work" << std::endl;
				empty.wait(lck,[&]{ return !data.empty() || !cont.load(); });
			};
			std::cout << "Thread notified" << std::endl;
		}

		if(!data.empty())
		{
			std::lock_guard<std::mutex> l(mutex_data);
			auto res = data.front();
			data.pop();
			return res;
		}
	};
	void abort(){
		std::cout << "Aborting" << std::endl;
		cont = false;
		empty.notify_all();
	};
};

#endif
