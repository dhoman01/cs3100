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
			thread_pairs.push_back(std::make_pair(i,avg_sd.first));
		}

		std::ofstream fout;
		std::stringstream ss;
		ss << n << "_search_time_results.txt";
		fout.open(ss.str());
		fout << "# of trials: " << trials << std::endl;
		fout << "# of threads    AVG " << std::endl;
		fout << "=================================================" << std::endl;
		std::for_each(thread_pairs.begin(), thread_pairs.end(), [=,&fout](auto& times){
			fout << times.first << "              " << times.second << std::endl;
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
	{
		std::lock_guard<std::mutex> lck(mutex_cout);
	}
	return right;
}

void quicksort(int start, int end, std::vector<int>& data, WorkQueue& wq)
{
	int r;
	if(end - start <= 50 && start < end)
	{
		{
			std::lock_guard<std::mutex> lck(mutex_cout);
		};
		std::lock_guard<std::mutex> lck(mutex_data);
		std::sort(data.begin() + start, data.begin() + end);
		count--;
		if(count.load() <= 0) cv.notify_all();
		return;
	}
  else if(start < end && end - start > 50)
  {
      r = partition(start,end,data);

			if(r - start < end - r)
			{
					count++;
					wq.post([=,&data,&wq](){quicksort(start,r-1,data,wq); count--;if(count.load() <= 0) cv.notify_all();});
		      quicksort(r+1,end,data,wq);
			}
			else
			{
					count++;
					wq.post([=,&data,&wq]{quicksort(r+1,end,data,wq);});
					quicksort(start,r-1,data,wq);
			}
  }
}

auto sort_experiment(auto trials)
{
	for(auto n = 100; n <= 1000000; n *= 10)
	{
		std::cout << "Gathering problem size " << n << std::endl;
		std::vector<std::pair<double,double>> thread_pairs;
		for(auto i = 1; i <= 8; i++)
		{	
			std::cout << "Threads: " << i << std::endl;
			std::vector<double> running_times (trials);
			for(auto t = 0; t < trials; t++)
			{
				std::vector<int> data;
				{
					std::lock_guard<std::mutex> lck(mutex_data);
					data = getRandomData(n);
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
							if(count.load() <= 0) cv.notify_all();
					};
					wq.post(qsort);

					while(count.load() > 0)
					{
						std::unique_lock<std::mutex> lck(mutex_wait);
						cv.wait(lck, [=]{
							return count.load() <= 0;
						});
					}
				};

				std::chrono::duration<double> time_span = timer::timeFunction<decltype(sort)>(sort);
				running_times.push_back(time_span.count());
			}
			std::pair<double, double> avg_sd = stats::getMeanAndSD(running_times);
			thread_pairs.push_back(std::make_pair(i,avg_sd.first));
		}

		std::ofstream fout;
		std::stringstream ss;
		ss << n << "_sort_time_results.txt";
		fout.open(ss.str());
		fout << "# of trials: " << trials << std::endl;
		fout << "# of threads    AVG " << std::endl;
		fout << "=================================================" << std::endl;
		std::for_each(thread_pairs.begin(), thread_pairs.end(), [=,&fout](auto& times){
			fout << times.first << "              " << times.second << std::endl;
		});
		
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

int main()
{
	auto trials = 100;
	std::cout << "Enter the number of trials: " << std::endl;
	std::cin >> trials;

	std::cout << "Doing search experiment" << std::endl;
	search_experiment(trials);
	std::cout << "search experiment finished..." << std::endl;
	
	std::cout << "Doing sort experiment" << std::endl;
	sort_experiment(trials);
	std::cout << "Sort experiment finsihed..." << std::endl;

	return EXIT_SUCCESS;
}
