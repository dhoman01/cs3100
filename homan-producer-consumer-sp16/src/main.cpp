// Author: Dustin Homan
// A#: A02177041
// Assignment-three (Producer/Consumer)
// This is the main driver and contains the
//     function that writes the image to a PPM
//     and executes the multiple threads
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <tuple>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include "mandelbrot.hpp"
#include "stats.hpp"
#include "timer.hpp"
#include "WorkQueue.hpp"

std::vector<std::vector<std::tuple<int,int,int>>> pixels;
std::mutex mutex_wait;
std::mutex mutex_pixel;
std::condition_variable finished;
std::atomic<bool> work_done(false);

// Write the image to a PPM
void writeToPpm(std::string name, auto imageWidth, auto imageHeight){
	// Open the output file and write the PPM header...
	std::ofstream fout(name.append("_output_image.ppm"));
	fout << "P3" << std::endl;                                                        // "Magic Number" - PPM file
	fout << imageWidth << " " << imageHeight << std::endl;    // Dimensions
	fout << "255" << std::endl;                                                      // Maximum value of a pixel R,G,B value...

	// Write to pixels
	for( auto y = 0; y < imageHeight; y++){
		for(auto x = 0; x < imageWidth; x++){
			std::lock_guard<std::mutex> l(mutex_pixel);
			auto color = pixels[x][y];                 // Color stored as tuple<Red, Green, Blue>
			fout << " " << std::get<0>(color) << " " << std::get<1>(color) << " " << std::get<2>(color) << " ";
		}
		fout << std::endl;
	}

	fout.close();
	return;
}

void chunks(auto trials)
{
	// File for storing timing data
	std::ofstream fout;
	fout.open("chunks_time_results.txt");
	std::vector<std::pair<int,double>> thread_avg_pairs;
	for(auto i = 1; i <= 8; i++)
	{
		std::vector<double> running_times;
		for(auto j = 0; j < trials; j++)
		{
			std::atomic<int> amount(0);
			work_done = false;
			const auto n = 32;

			auto minX = 0;
			auto maxX = mandelbrot::imageWidth / n;
			auto minY = 0;
			auto maxY = mandelbrot::imageHeight;

            {
                std::lock_guard<std::mutex> lck(mutex_pixel);
                pixels = std::vector<std::vector<std::tuple<int,int,int>>> (mandelbrot::imageWidth, std::vector<std::tuple<int,int,int>> (mandelbrot::imageHeight, std::make_tuple(0,0,0)));
			};
			auto f = [&](){
				WorkQueue wq;

				wq.start(i);

				while(maxX <= mandelbrot::imageWidth)
				{
					wq.post([&,minX,maxX,maxY](){
						{
							mandelbrot::generateSet(minX, maxX, 0, maxY, pixels, mutex_pixel);
							amount.fetch_add(1);
							std::cout << "amount complete: " << amount.load() << "/" << n << std::endl;;
						};
						if(amount.load() == n){
							std::cout << "Work is done!" << std::endl;
							work_done.store(true);
							std::cout << "Notifying main" << std::endl;
							finished.notify_all();
						}
					});
					minX = maxX;
					maxX += mandelbrot::imageWidth / n;
				}

				std::cout << "All the work has been added" << std::endl;
				while(!(work_done.load()))
				{
					{
						std::unique_lock<std::mutex> l(mutex_wait);
						finished.wait(l, [] {
							return work_done.load();
						});
					};
				}
			};
			std::chrono::duration<double> time_span = timer::timeFunction<decltype(f)>(f);

			// Store the running time for further calculations
			running_times.push_back(time_span.count());
		}
		// Calculate the average and standard deviation of the trials
		std::pair<double,double> avg_sd = stats::getMeanAndSD(running_times);
		thread_avg_pairs.push_back(std::make_pair(i,avg_sd.first));

		fout << "=======================================\n";
		fout << "Image stats: imageWidth, imageHeight, maxN, minR, maxR, minI, maxI\n";
		fout << mandelbrot::imageWidth << ", " << mandelbrot::imageHeight << ", " << mandelbrot::maxN;
		fout << ", " << mandelbrot::minR << ", " << mandelbrot::maxR << ", " << mandelbrot::minI << ", " << mandelbrot::maxI << std::endl;
		fout << "# of trials: " << trials << std::endl;
		fout << "# of threads: " << i << std::endl;
		fout << "Threaded average running time: " << avg_sd.first << "\n";
		fout << "Threaded standard deviation: " << avg_sd.second << "\n";
		fout << "=======================================\n\n";
	}
	double org_avg = thread_avg_pairs[0].second;
	std::for_each(thread_avg_pairs.begin(), thread_avg_pairs.end(), [&fout,org_avg](auto& pair){
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

void perPixel(auto trials)
{
	// File for storing timing data
	std::ofstream fout;
	fout.open("per_pixel_time_results.txt");
	std::vector<std::pair<int, double>> thread_avg_pairs;
	for (auto i = 1; i <= 8; i++)
	{
		std::vector<double> running_times;
		for (auto j = 0; j < trials; j++)
		{
			std::atomic<int> amount(0);
			work_done = false;

			pixels = std::vector<std::vector<std::tuple<int, int, int>>>(mandelbrot::imageWidth, std::vector<std::tuple<int, int, int>>(mandelbrot::imageHeight, std::make_tuple(0, 0, 0)));

			auto f = [&](){
				WorkQueue wq;

				wq.start(i);
				auto total = mandelbrot::imageWidth * mandelbrot::imageHeight;
				for (int x = 0; x < mandelbrot::imageWidth; x++)
				{
					for (int y = 0; y < mandelbrot::imageHeight; y++)
					{
						wq.post([=, &amount]() {
							mandelbrot::generateSet(x, x + 1, y, y + 1, pixels, mutex_pixel);
							amount++;
							std::cout << "amount complete: " << amount << "/" << total << std::endl;
							if (amount == total)
							{
								std::cout << "Work is done!" << std::endl;
								work_done = true;
								std::cout << "Notifying main" << std::endl;
								finished.notify_all();
							}
						});
					}
				}

				std::cout << "All the work has been added" << std::endl;
				while (!(work_done.load()))
				{
					{
						std::unique_lock<std::mutex> l(mutex_wait);
						finished.wait(l, [] {
							return work_done.load();
						});
					};
				}
			};
			std::chrono::duration<double> time_span = timer::timeFunction<decltype(f)>(f);

			// Store the running time for further calculations
			running_times.push_back(time_span.count());
		}
		// Calculate the average and standard deviation of the trials
		std::pair<double, double> avg_sd = stats::getMeanAndSD(running_times);
		thread_avg_pairs.push_back(std::make_pair(i, avg_sd.first));

		fout << "=======================================\n";
		fout << "Image stats: imageWidth, imageHeight, maxN, minR, maxR, minI, maxI\n";
		fout << mandelbrot::imageWidth << ", " << mandelbrot::imageHeight << ", " << mandelbrot::maxN;
		fout << ", " << mandelbrot::minR << ", " << mandelbrot::maxR << ", " << mandelbrot::minI << ", " << mandelbrot::maxI << std::endl;
		fout << "# of trials: " << trials << std::endl;
		fout << "# of threads: " << i << std::endl;
		fout << "Threaded average running time: " << avg_sd.first << "\n";
		fout << "Threaded standard deviation: " << avg_sd.second << "\n";
		fout << "=======================================\n\n";
	}
	double org_avg = thread_avg_pairs[0].second;
	std::for_each(thread_avg_pairs.begin(), thread_avg_pairs.end(), [&fout, org_avg](auto& pair) {
		fout << "=======================================\n";
		fout << "Comparing 1 thread to " << pair.first << " threads" << std::endl;
		double S = stats::getSpeedUp(org_avg, pair.second);
		fout << "Speed up: " << S << std::endl;
		double E = stats::getEfficiency(S, pair.first);
		fout << "Efficiency: " << E << std::endl;
		fout << "=======================================\n\n";
	});

	fout.close();
}

void multPixel(int trials)
{
	// File for storing timing data
	std::ofstream fout;
	fout.open("mult_pixel_time_results.txt");
	std::vector<std::pair<int, double>> thread_avg_pairs;
	for (auto i = 1; i <= 8; i++)
	{
		std::vector<double> running_times;
		for (auto j = 0; j < trials; j++)
		{
			std::atomic<int> amount(0);
			work_done = false;

			auto n = 32;

			pixels = std::vector<std::vector<std::tuple<int, int, int>>>(mandelbrot::imageWidth, std::vector<std::tuple<int, int, int>>(mandelbrot::imageHeight, std::make_tuple(0, 0, 0)));

			auto f = [&]() {
				WorkQueue wq;

				wq.start(i);

				for (int y = 0; y < mandelbrot::imageHeight; y++)
				{
					auto minX = 0;
					auto maxX = mandelbrot::imageWidth / n;
					while (maxX <= mandelbrot::imageWidth)
					{
						wq.post([=, &amount]() {
							{
								mandelbrot::generateSet(minX, maxX, y, y + 1, pixels, mutex_pixel);
								amount++;
								std::cout << "amount complete: " << amount << "/" << n * mandelbrot::imageHeight << std::endl;;
							};
							if (amount == n * mandelbrot::imageHeight) {
								std::cout << "Work is done!" << std::endl;
								work_done = true;
								std::cout << "Notifying main" << std::endl;
								finished.notify_all();
							}
						});

						minX = maxX;
						maxX += mandelbrot::imageWidth / n;
					}
				}

				std::cout << "All the work has been added" << std::endl;
				while (!(work_done.load()))
				{
					{
						std::unique_lock<std::mutex> l(mutex_wait);
						finished.wait(l, [] {
							return work_done.load();
						});
					};
				}
			};
			std::chrono::duration<double> time_span = timer::timeFunction<decltype(f)>(f);

			// Store the running time for further calculations
			running_times.push_back(time_span.count());
		}
		// Calculate the average and standard deviation of the trials
		std::pair<double, double> avg_sd = stats::getMeanAndSD(running_times);
		thread_avg_pairs.push_back(std::make_pair(i, avg_sd.first));

		fout << "=======================================\n";
		fout << "Image stats: imageWidth, imageHeight, maxN, minR, maxR, minI, maxI\n";
		fout << mandelbrot::imageWidth << ", " << mandelbrot::imageHeight << ", " << mandelbrot::maxN;
		fout << ", " << mandelbrot::minR << ", " << mandelbrot::maxR << ", " << mandelbrot::minI << ", " << mandelbrot::maxI << std::endl;
		fout << "# of trials: " << trials << std::endl;
		fout << "# of threads: " << i << std::endl;
		fout << "Threaded average running time: " << avg_sd.first << "\n";
		fout << "Threaded standard deviation: " << avg_sd.second << "\n";
		fout << "=======================================\n\n";
	}
	double org_avg = thread_avg_pairs[0].second;
	std::for_each(thread_avg_pairs.begin(), thread_avg_pairs.end(), [&fout, org_avg](auto& pair) {
		fout << "=======================================\n";
		fout << "Comparing 1 thread to " << pair.first << " threads" << std::endl;
		double S = stats::getSpeedUp(org_avg, pair.second);
		fout << "Speed up: " << S << std::endl;
		double E = stats::getEfficiency(S, pair.first);
		fout << "Efficiency: " << E << std::endl;
		fout << "=======================================\n\n";
	});

	fout.close();
}

void rows(auto trials)
{
	// File for storing timing data
	std::ofstream fout;
	fout.open("rows_time_results.txt");
	std::vector<std::pair<int, double>> thread_avg_pairs;
	for (auto i = 1; i <= 8; i++)
	{
		std::vector<double> running_times;
		for (auto j = 0; j < trials; j++)
		{
			work_done = false;
			std::atomic<int> amount(0);

			pixels = std::vector<std::vector<std::tuple<int, int, int>>>(mandelbrot::imageWidth, std::vector<std::tuple<int, int, int>>(mandelbrot::imageHeight, std::make_tuple(0, 0, 0)));
			auto f = [&]() {
				WorkQueue wq;

				wq.start(i);
				auto maxX = mandelbrot::imageWidth;
				for (int y = 0; y < mandelbrot::imageHeight; y++)
				{
					wq.post([=, &amount]() {
						{
							mandelbrot::generateSet(0, maxX, y, y + 1, pixels, mutex_pixel);
							amount++;
							std::cout << "amount complete: " << amount << "/" << mandelbrot::imageHeight << std::endl;;
						};
						if (amount == mandelbrot::imageHeight) {
							std::cout << "Work is done! " << amount << std::endl;
							work_done = true;
							std::cout << "Notifying main" << std::endl;
							finished.notify_all();
						}
					});
				}

				std::cout << "All the work has been added" << std::endl;
				while (!(work_done.load()))
				{
					{
						std::unique_lock<std::mutex> l(mutex_wait);
						finished.wait(l, [] {
							return work_done.load();
						});
					};
				}
			};
			std::chrono::duration<double> time_span = timer::timeFunction<decltype(f)>(f);

			// Store the running time for further calculations
			running_times.push_back(time_span.count());
		}
		// Calculate the average and standard deviation of the trials
		std::pair<double, double> avg_sd = stats::getMeanAndSD(running_times);
		thread_avg_pairs.push_back(std::make_pair(i, avg_sd.first));

		fout << "=======================================\n";
		fout << "Image stats: imageWidth, imageHeight, maxN, minR, maxR, minI, maxI\n";
		fout << mandelbrot::imageWidth << ", " << mandelbrot::imageHeight << ", " << mandelbrot::maxN;
		fout << ", " << mandelbrot::minR << ", " << mandelbrot::maxR << ", " << mandelbrot::minI << ", " << mandelbrot::maxI << std::endl;
		fout << "# of trials: " << trials << std::endl;
		fout << "# of threads: " << i << std::endl;
		fout << "Threaded average running time: " << avg_sd.first << "\n";
		fout << "Threaded standard deviation: " << avg_sd.second << "\n";
		fout << "=======================================\n\n";
	}
	double org_avg = thread_avg_pairs[0].second;
	std::for_each(thread_avg_pairs.begin(), thread_avg_pairs.end(), [&fout, org_avg](auto& pair) {
		fout << "=======================================\n";
		fout << "Comparing 1 thread to " << pair.first << " threads" << std::endl;
		double S = stats::getSpeedUp(org_avg, pair.second);
		fout << "Speed up: " << S << std::endl;
		double E = stats::getEfficiency(S, pair.first);
		fout << "Efficiency: " << E << std::endl;
		fout << "=======================================\n\n";
	});

	fout.close();
}

void even(int trials)
{
	// File for storing timing data
	std::ofstream fout;
	fout.open("even_time_results.txt");
	std::vector<std::pair<int, double>> thread_avg_pairs;
	for (auto i = 1; i <= 8; i++)
	{
		std::vector<double> running_times;
		for (auto j = 0; j < trials; j++)
		{
			std::atomic<int> amount(0);
			auto n = i;
			auto minX = 0, maxX = mandelbrot::imageWidth / n, minY = 0, maxY = mandelbrot::imageHeight;

			pixels = std::vector<std::vector<std::tuple<int, int, int>>>(mandelbrot::imageWidth, std::vector<std::tuple<int, int, int>>(mandelbrot::imageHeight, std::make_tuple(0, 0, 0)));

			auto f = [&]() {
				WorkQueue wq;

				wq.start(n);
				while (maxX <= mandelbrot::imageWidth)
				{
					wq.post([=, &amount]() {
						{
							mandelbrot::generateSet(minX, maxX, 0, maxY, pixels, mutex_pixel);
							amount++;
							std::cout << "amount complete: " << amount << "/" << n << std::endl;;
						};
						if (amount == n) {
							std::cout << "Work is done!" << std::endl;
							work_done = true;
							std::cout << "Notifying main" << std::endl;
							finished.notify_all();
						}
					});
					minX = maxX;
					maxX += mandelbrot::imageWidth / n;
				}

				std::cout << "All the work has been added" << std::endl;
				while (!(work_done.load()))
				{
					{
						std::unique_lock<std::mutex> l(mutex_wait);
						finished.wait(l, [] {
							return work_done.load();
						});
					};
				}
			};
			std::chrono::duration<double> time_span = timer::timeFunction<decltype(f)>(f);

			// Store the running time for further calculations
			running_times.push_back(time_span.count());
		}
		// Calculate the average and standard deviation of the trials
		std::pair<double, double> avg_sd = stats::getMeanAndSD(running_times);
		thread_avg_pairs.push_back(std::make_pair(i, avg_sd.first));

		fout << "=======================================\n";
		fout << "Image stats: imageWidth, imageHeight, maxN, minR, maxR, minI, maxI\n";
		fout << mandelbrot::imageWidth << ", " << mandelbrot::imageHeight << ", " << mandelbrot::maxN;
		fout << ", " << mandelbrot::minR << ", " << mandelbrot::maxR << ", " << mandelbrot::minI << ", " << mandelbrot::maxI << std::endl;
		fout << "# of trials: " << trials << std::endl;
		fout << "# of threads: " << i << std::endl;
		fout << "Threaded average running time: " << avg_sd.first << "\n";
		fout << "Threaded standard deviation: " << avg_sd.second << "\n";
		fout << "=======================================\n\n";
	}
	double org_avg = thread_avg_pairs[0].second;
	std::for_each(thread_avg_pairs.begin(), thread_avg_pairs.end(), [&fout, org_avg](auto& pair) {
		fout << "=======================================\n";
		fout << "Comparing 1 thread to " << pair.first << " threads" << std::endl;
		double S = stats::getSpeedUp(org_avg, pair.second);
		fout << "Speed up: " << S << std::endl;
		double E = stats::getEfficiency(S, pair.first);
		fout << "Efficiency: " << E << std::endl;
		fout << "=======================================\n\n";
	});

	fout.close();
}

int main(){
	auto trials = 5;
	std::cout << "Please enter the appropriate values for imageWidth, imageHeight, maxN, minR, maxR, minI, maxI\n";
	std::cin >> mandelbrot::imageWidth >> mandelbrot::imageHeight >> mandelbrot::maxN >> mandelbrot::minR >> mandelbrot::maxR >> mandelbrot::minI >> mandelbrot::maxI;

	 std::cout << "Please enter the number of trials to find the avg and sd\n";
	 std::cin >> trials;
	std::cout << "Chunks" << std::endl;
	chunks(trials);
    // Write generated image to PPM
	std::cout << "Finished generating image, writing to file...\n";
	writeToPpm("chunks",mandelbrot::imageWidth, mandelbrot::imageHeight);
	std::cout << "Finished writing to file.\n";

    std::cout << "Per pixel" << std::endl;
    perPixel(trials);
    //// Write generated image to PPM
    std::cout << "Finished generating image, writing to file...\n";
    writeToPpm("per_pixel",mandelbrot::imageWidth, mandelbrot::imageHeight);
    std::cout << "Finished writing to file.\n";

    //work_done = false;
	std::cout << "multPixel" << std::endl;
    multPixel(trials);
    // Write generated image to PPM
	std::cout << "Finished generating image, writing to file...\n";
    writeToPpm("mult_pixel",mandelbrot::imageWidth, mandelbrot::imageHeight);
	std::cout << "Finished writing to file.\n";

	std::cout << "rows" << std::endl;
    rows(trials);

    // Write generated image to PPM
	std::cout << "Finished generating image, writing to file...\n";
    writeToPpm("rows",mandelbrot::imageWidth, mandelbrot::imageHeight);
	std::cout << "Finished writing to file.\n";

	std::cout << "even" << std::endl;
    even(trials);
    // Write generated image to PPM
	std::cout << "Finished generating image, writing to file...\n";
    writeToPpm("even",mandelbrot::imageWidth, mandelbrot::imageHeight);
	std::cout << "Finished writing to file.\n";


	return EXIT_SUCCESS;
}
