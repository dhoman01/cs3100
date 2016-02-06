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
				throw std::runtime_error("Wait aborted");
			}
			{
				std::unique_lock<std::mutex> lck (mutex_wait);
				empty.wait(lck,[=]{ return !data.empty() || !cont.load(); });
			};
		}

		std::lock_guard<std::mutex> l(mutex_data);
		if(!data.empty())
		{
			auto res = data.front();
			if(!res) std::cout << "res is empty and data.empty() is " << data.empty() << std::endl;
			data.pop();
			return res;
		}
	};
	void abort(){
		cont = false;
		empty.notify_all();
	};
};

#endif
