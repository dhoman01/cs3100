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
		data.push_back(std::rand() % std::max(1,size) + 1);
	}

	return data;
}

auto search_experiment(auto trials)
{
	for(auto n = 100; n <= 1000000; n *= 10)
	{
		std::cout << "Gathering problem size " << n << std::endl;
		std::vector<int> data;
		{
			std::lock_guard<std::mutex> lck(mutex_data);
			data = getRandomData(n);
		};
		auto min = 0;
		auto max = 0;
		auto target = data[std::rand() % data.size()];

		std::vector<std::pair<double,double>> thread_pairs;
		for(auto i = 1; i <= 8; i++)
		{
			std::cout << "threads " << i << std::endl;
			std::vector<double> running_times (trials);
			for(auto t = 0; t < trials; t++)
			{
				auto search = [&](){
					WorkQueue wq;

					wq.start(i);

					min = 0;

					max = data.size() / i;

					count = 0;
					while(max <= data.size() )
					{
						wq.post([=](){
							std::lock_guard<std::mutex> lck(mutex_data);
							auto res = std::find(data.begin() + min, data.begin() + max + 1, target);
							count++;
							if(count.load() == i) cv.notify_all();
						});
						min = max + 1;
						max += data.size() / i;
					}

					while(count.load() < i)
					{
						std::unique_lock<std::mutex> lck(mutex_wait);
						cv.wait(lck, [=]{
							bool done = count.load() == i;
							return done;
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
		fout << "\n\n";
		double org_avg = thread_pairs[0].second;
		std::for_each(thread_pairs.begin(), thread_pairs.end(), [&fout,org_avg](auto& pair){
			fout << "=======================================\n";
			fout << "Comparing 1 thread to " << pair.first << " threads" << std::endl;
			double S =  stats::getSpeedUp(org_avg, pair.second);
			fout << "Speed up: " << S << std::endl;
			double E = stats::getEfficiency(S, pair.first);
			fout << "Efficiency: " << E << std::endl;
			fout << "=======================================\n\n";
		});

		fout.close();
	}
}

int partition(int start, int end, std::vector<int>& data)
{
	int mid = (end - start) / 2;
	{
		std::lock_guard<std::mutex> lck(mutex_data);
		if(data[start] < data[mid] && data[mid] < data[end]) std::swap(data[mid], data[start]);
		if(data[end] < data[start] && data[end] < data[mid]) std::swap(data[end], data[start]);
	}
	int left = start;
	int right = end;
	while(left < right)
	{
		std::lock_guard<std::mutex> lck(mutex_data);
		while(data[left] <= data[start] && left != end) left++;
		while(data[right] > data[start] && right != start) right--;
		if(left < right) std::swap(data[left], data[right]);
	}
	{
		std::lock_guard<std::mutex> lck(mutex_data);
		std::swap(data[start], data[right]);
	}

	return right;
}

void quicksort(int start, int end, std::vector<int>& data, WorkQueue& wq)
{
	// {
	// 	std::lock_guard<std::mutex> lck(mutex_cout);
	// 	std::cout << "Data before:" << std::endl;
	// 	{
	// 		std::lock_guard<std::mutex> lck(mutex_data);
	// 		std::for_each(data.begin(), data.end(), [](auto& item){
	// 			std::cout << item << " ";
	// 		});
	// 	};
	// 	std::cout << std::endl;
	// };
	int r;
	if(end - start <= 50 && start < end)
	{
		std::lock_guard<std::mutex> lck(mutex_data);
		std::sort(data.begin() + start, data.begin() + end);
		count--;
		if(count.load() <= 0) cv.notify_all();
	}
  else if(start<end && end - start > 50)
  {
      r = partition(start,end,data);
			//std::cout << "partition " << r << std::endl;
			if((r-1) - start < end - (r + 1))
			{
				count++;
				wq.post([=,&data,&wq](){quicksort(start,r-1,data,wq);});
	      quicksort(r+1,end,data,wq);
			}
			else
			{
				count++;
				wq.post([=,&data,&wq]{quicksort(r+1,end,data,wq);});
				quicksort(start,r-1,data,wq);
			}
  }
	// {
	// 	std::lock_guard<std::mutex> lck(mutex_cout);
	// 	std::cout << "Data after:" << std::endl;
	// 	{
	// 		std::lock_guard<std::mutex> lck(mutex_data);
	// 		std::for_each(data.begin(), data.end(), [](auto& item){
	// 			std::cout << item << " ";
	// 		});
	// 	};
	// 	std::cout << std::endl;
	// };
}

// auto partition_check(){
// 	std::vector<int> data = getRandomData(100);
// 	std::cout << "Data before partition:" << std::endl;
// 	std::for_each(data.begin(), data.end(), [](auto& item){
// 		std::cout << item << " ";
// 	});
// 	std::cout << std::endl;
// 	quicksort(0, data.size() - 1, data);
//
// 	std::cout << "Data after partition:" << std::endl;
// 	std::for_each(data.begin(), data.end(), [](auto& item){
// 		std::cout << item << " ";
// 	});
// 	std::cout << std::endl;
// }

auto sort_experiment(auto trials)
{
	for(auto n = 100; n <= 1000000; n *= 10)
	{
		std::vector<std::pair<double,double>> thread_pairs;
		for(auto i = 1; i <= 8; i++)
		{
			std::cout << "threads " << i << std::endl;
			std::vector<double> running_times (trials);
			for(auto t = 0; t < trials; t++)
			{
				std::cout << "Trials " << t << std::endl;
				std::vector<int> data;
				{
					std::lock_guard<std::mutex> lck(mutex_data);
					data = getRandomData(n);
				};
				{
					std::lock_guard<std::mutex> lck(mutex_cout);
					std::cout << "Data before:" << std::endl;
					{
						std::lock_guard<std::mutex> lck(mutex_data);
						std::for_each(data.begin(), data.end(), [](auto& item){
							std::cout << item << " ";
						});
					};
					std::cout << std::endl;
				};
				count = 1;
				auto sort = [&](){

					WorkQueue wq;

					wq.start(i);
					auto start = 0;
					auto end = data.size();

					auto qsort  = [=,&data,&wq](){
							quicksort(start,end,data,wq);
							count--;
					};
					wq.post(qsort);

					while(count.load() > 0)
					{
						std::unique_lock<std::mutex> lck(mutex_wait);
						cv.wait(lck, [=]{
							return count.load() <= 0;
						});
					}
					{
						std::lock_guard<std::mutex> lck(mutex_cout);
						std::cout << "Data after:" << std::endl;
						{
							std::lock_guard<std::mutex> lck(mutex_data);
							std::for_each(data.begin(), data.end(), [](auto& item){
								std::cout << item << " ";
							});
						};
						std::cout << std::endl;
					};
				};

				std::chrono::duration<double> time_span = timer::timeFunction<decltype(sort)>(sort);
				running_times.push_back(time_span.count());
			}
			std::pair<double, double> avg_sd = stats::getMeanAndSD(running_times);
			thread_pairs.push_back(avg_sd);
		}

		std::ofstream fout;
		std::stringstream ss;
		ss << n << "_sort_time_results.txt";
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
	// std::cout << "Doing search experiment" << std::endl;
	// search_experiment(trials);
	// std::cout << "search experiment finished..." << std::endl;
	//partition_check();

	sort_experiment(trials);

	return EXIT_SUCCESS;
}
