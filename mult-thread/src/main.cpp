#include <iostream>
#include <stdlib.h>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "WorkQueue.cpp"
#include <chrono>

std::mutex mutex_i;
std::mutex mut;
std::atomic<int> i (0);
std::condition_variable cv;

void inc()
{
	i++;
}

int main()
{
	WorkQueue wq;
	
	std::cout << "Starting work queue" << std::endl;
	wq.start(4);
	for(auto j = 0; j < 100; j++)
	{	
		wq.post([](){
			{
				std::lock_guard<std::mutex> lck(mutex_i);
				std::cout << "Doing task " << i << std::endl;
				i++;
				if(i==99) cv.notify_one();
			};
		});
	}
	
	std::cout << "Initializing cv" << std::endl;
				
	std::cout << "Going into wait loop" << std::endl;
	while(i.load() != 100)
	{
		{
			std::unique_lock<std::mutex> lck(mutex_i);
			std::cout << "calling cv.wait with i = " << i.load() << std::endl;
			cv.wait(lck, [](){ return i == 100; });
		};
		
		std::cout << "cv was notified" << std::endl;
	}
	

	return EXIT_SUCCESS;
}
