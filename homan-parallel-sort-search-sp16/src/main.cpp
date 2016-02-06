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
#include <sstream>
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

auto search_experiment(auto trials)
{
	for(auto n = 100; n <= 1000000; n *= 10)
	{
		std::vector<int> data;
		{
			std::lock_guard<std::mutex> lck(mutex_data);
			data = getRandomData(1000000);
		};
		auto min = 0;
		auto max = 0;
		auto target = data[std::rand() % data.size()];
		auto finished = 0;
		std::vector<std::pair<double,double>> thread_pairs;
		for(auto i = 1; i <= 8; i++)
		{
			std::vector<double> running_times (trials);
			for(auto t = 0; t < trials; t++)
			{
				auto search = [&](){

					while(max <= data.size() )
					{
						WorkQueue wq;

						wq.start(i);

						finished = i;
						min = 0;

						max = data.size() / finished;

						count = 0;
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
				running_times.push_back(time_span.count());
			}
			std::pair<double, double> avg_sd = stats::getMeanAndSD(running_times);
			thread_pairs.push_back(avg_sd);
		}

		std::ofstream fout;
		std::stringstream ss;
		ss << n << "_search_time_results.txt";
		fout.open(ss.str());
		fout << "# of trials: " << trials << std::endl;
		fout << "# of threads    AVG               SD" << std::endl;
		fout << "=================================================" << std::endl;
		int i = 1;
		std::for_each(thread_pairs.begin(), thread_pairs.end(), [=,&fout,&i](auto& times){
			fout << i << "               " << times.first << "          " << times.second << std::endl;
			i++;
		});

		fout.close();
	}
}

int main()
{
	auto trials = 100;
	std::cout << "Enter the number of trials: " << std::endl;
	std::cin >> trials;

	search_experiment(trials);

	return EXIT_SUCCESS;
}
