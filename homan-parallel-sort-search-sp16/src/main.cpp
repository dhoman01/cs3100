#include <iostream>
#include <fstream>
#include <thread>
#include <stdlib.h>
#include <time.h>
#include <mutex>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <algorithm>
#include "WorkQueue.hpp"
#include "timer.hpp"
#include "stats.hpp"

std::mutex mutex_data;
std::mutex mutex_wait;
std::mutex mutex_cout;
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
	std::vector<std::vector<double>> running_times;
	for(auto t = 0; t < 100; t++)
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
			std::cout << "wq" << std::endl;
			WorkQueue wq;
			std::cout << "wq.start()" << std::endl;
			wq.start(i);
			
			finished = i;
			min = 0;
			
			max = data.size() / finished;
			
			count = 0;
			running_times.push_back(std::vector<double>());
			auto search = [&](){

				while(max <= data.size() )
				{
					wq.post([=](){
						{
							std::lock_guard<std::mutex> lck(mutex_data);
							auto res = std::find(data.begin() + min, data.begin() + max, target);
						};
						count++;
						if(count.load() == finished) cv.notify_all();
					});
					min = max;
					max += data.size() / finished;
				}

				while(count.load() < finished)
				{
					std::unique_lock<std::mutex> lck(mutex_wait);
					cv.wait(lck, [=]{
						return count.load() == finished;
					});
				}
			};
			
			std::chrono::duration<double> time_span = timer::timeFunction<decltype(search)>(search);
			running_times[i].push_back(time_span.count());
			
			std::cout << "ending iteration" << std::endl;
			
			std::this_thread::sleep_for(std::chrono::seconds(2));
		}
	}
	std::ofstream fout;
	fout.open("time_results.txt");
	fout << "AVG     SD" << std::endl;
	fout << "=============" << std::endl;
	std::for_each(running_times.begin(), running_times.end(), [=,&fout](auto& times){
		std::pair<double, double> avg_sd = stats::getMeanAndSD(times);
		fout << avg_sd.first << "     " << avg_sd.second << std::endl;
	});
	
	fout.close();

	return EXIT_SUCCESS;
}
