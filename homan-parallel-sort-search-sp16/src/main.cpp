#include <iostream>
#include <fstream>
#include <thread>
#include <stdlib.h>
#include <time.h>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "WorkQueue.hpp"

std::mutex mutex_data;
std::mutex mutex_wait;
std::atomic<int> count(0);
std::condition_variable cv;

auto getRandomData(int size)
{
	std::vector<int> data;
	std::srand(std::time(NULL));
	for(auto i = 0; i < size; i++)
	{
		data.push_back(std::rand() % size + 1);
	}
	
	return data;
}

int main()
{	
	for(int i = 1; i <= 8; i++)
	{
		WorkQueue wq;
		wq.start(i);
		std::cout << i << " threads" << std::endl;
		std::vector<int> data;
		{
			std::lock_guard<std::mutex> lck(mutex_data);
			data = getRandomData(10 * (std::pow(10,i)));
		};
		auto min = 0;
		auto max = data.size() / i;
		auto target = data[std::rand() % data.size()];
		while(max <= data.size() )
		{
			wq.post([=,&data](){
				auto res = std::find(data.begin() + min, data.begin() + max, target);
				std::cout << "target: " << target << "      result: " << *res << std::endl;
				count++;
				if(count == i) cv.notify_all();
			});
			min = max;
			max += data.size() / i;
		}		
		
		std::cout << "Ending..." << std::endl;
		
		while(count.load() < i)
		{
			std::unique_lock<std::mutex> lck(mutex_wait);
			cv.wait(lck, [=]{
				return count.load() == i;
			});
		}
	}
	
	return EXIT_SUCCESS;
}
