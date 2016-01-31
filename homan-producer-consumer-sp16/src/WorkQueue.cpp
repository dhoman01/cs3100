#include "WorkQueue.hpp"
#include <algorithm>

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
