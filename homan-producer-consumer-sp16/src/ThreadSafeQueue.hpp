#include <mutex>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <exception>

template<class T>
class ThreadSafeQueue{
private:
	std::queue<T> data;
	std::mutex mutex_data;
	std::mutex mutex_empty;
	std::mutex mutex_cont;
	std::condition_variable notEmpty;
	std::atomic<bool> cont;
public:
	ThreadSafeQueue():data(),mutex_data(),notEmpty(),mutex_cont(),cont(true){};
	ThreadSafeQueue(const ThreadSafeQueue& tsq) : data(tsq.data),mutex_data(),notEmpty(),mutex_cont(),cont(true){};
	void enqueue(T t)
	{
		std::lock_guard<std::mutex> l(mutex_data);
		data.push(t);
		notEmpty.notify_one();
	};
	T dequeue()
	{
		while(data.empty())
		{
			if(!cont.load()) throw std::runtime_error("Wait aborted");
			{
				std::unique_lock<std::mutex> lck (mutex_empty);
				bool empty = data.empty();
				std::cout << "Calling wait on thread" << std::endl;
				notEmpty.wait(lck,[empty]{ return !empty; });
			};
			
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
		std::lock_guard<std::mutex> l(mutex_cont);
		std::cout << "Aborting" << std::endl;
		cont = false;
		std::cout << "Notifing all" << std::endl;
		notEmpty.notify_all();
	};
};
