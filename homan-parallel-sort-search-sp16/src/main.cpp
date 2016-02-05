#include <iostream>
#include <fstream>
#include <thread>
#include <stdlib.h>
#include <time.h>
#include <mutex>
#include <atomic>
#include <cmath>
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
	std::vector<int> data;
	{
		std::lock_guard<std::mutex> lck(mutex_data);
		data = getRandomData(10000);
	};
	auto min = 0;
	auto max = 0;
	auto target = data[std::rand() % data.size()];
	auto finished = 0;
	for(auto i = 1; i <= 8; i++)
	{
		WorkQueue wq;
		wq.start(i);
		finished = i;
		std::cout << finished << " threads" << std::endl;
		min = 0;
		max = data.size() / finished;
		count = 0;
		while(max <= data.size() )
		{
			wq.post([&,min,max,target,finished](){
				auto res = std::find(data.begin() + min, data.begin() + max, target);
				//std::cout << "target: " << target << "      result: " << *res << std::endl;
				count++;
				if(count.load() == finished) cv.notify_all();
			});
			min = max;
			max += data.size() / finished;
		}

		std::cout << "Ending..." << std::endl;

		while(count.load() < finished)
		{
			std::unique_lock<std::mutex> lck(mutex_wait);
			cv.wait(lck, [=]{
				return count.load() == finished;
			});
		}
	}

	return EXIT_SUCCESS;
}
