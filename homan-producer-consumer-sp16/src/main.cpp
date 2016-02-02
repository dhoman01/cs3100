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
			fout << " " << std::get<0>(color) << " " << std::<1>(color) << " " << std::get<2>(color) << " ";
		}
		fout << std::endl;
	}

	fout.close();
	return;
}

void chunks()
{
	std::atomic<int> amount(0);
	auto n = 32;

	auto minX = 0;
	auto maxX = mandelbrot::imageWidth / n;
	auto minY = 0;
	auto maxY = mandelbrot::imageHeight;

	pixels = std::vector<std::vector<std::tuple<int,int,int>>> (mandelbrot::imageWidth, std::vector<std::tuple<int,int,int>> (mandelbrot::imageHeight, std::make_tuple(0,0,0)));

	WorkQueue wq;

	wq.start(4);

	while(maxX <= mandelbrot::imageWidth)
	{
		wq.post([=,&amount](){
			{
				mandelbrot::generateSet(minX, maxX, 0, maxY, pixels);
				amount++;
				std::cout << "amount complete: " << amount << "/" << n << std::endl;;
			};
			if(amount == n){
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
	while(!(work_done.load()))
	{
		{
			std::unique_lock<std::mutex> l(mutex_wait);
			finished.wait(l, [] {
				return work_done.load();
			});
		};
	}
}

void perPixel()
{
	std::atomic<int> amount(0);

	pixels = std::vector<std::vector<std::tuple<int,int,int>>> (mandelbrot::imageWidth, std::vector<std::tuple<int,int,int>> (mandelbrot::imageHeight, std::make_tuple(0,0,0)));

	WorkQueue wq;

	wq.start(4);
    auto total = mandelbrot::imageWidth * mandelbrot::imageHeight;
	for(int x = 0; x < mandelbrot::imageWidth; x++)
	{
		for(int y = 0; y < mandelbrot::imageHeight; y++)
		{
			wq.post([=,&amount](){
                mandelbrot::generateSet(x, x + 1, y, y + 1, pixels);
                amount++;
				std:: << "amount complete: " << amount << "/" << total << std::endl;
				if(amount == total)
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
	while(!(work_done.load()))
	{
		{
			std::unique_lock<std::mutex> l(mutex_wait);
			finished.wait(l, [] {
				return work_done.load();
			});
		};
	}
}

void multPixel()
{
	std::atomic<int> amount(0);

	auto n = 32;

	pixels = std::vector<std::vector<std::tuple<int,int,int>>> (mandelbrot::imageWidth, std::vector<std::tuple<int,int,int>> (mandelbrot::imageHeight, std::make_tuple(0,0,0)));

	WorkQueue wq;

	wq.start(4);

	for(int y = 0; y < mandelbrot::imageHeight; y++)
	{
		auto minX = 0;
		auto maxX = mandelbrot::imageWidth / n;
		while(maxX <= mandelbrot::imageWidth)
		{
			wq.post([=,&amount](){
				{
					mandelbrot::generateSet(minX, maxX, y, y + 1, pixels);
					amount++;
					std::cout << "amount complete: " << amount << "/" << n * mandelbrot::imageHeight << std::endl;;
				};
				if(amount == n * mandelbrot::imageHeight){
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
	while(!(work_done.load()))
	{
		{
			std::unique_lock<std::mutex> l(mutex_wait);
			finished.wait(l, [] {
				return work_done.load();
			});
		};
	}
}

void rows()
{
	std::atomic<int> amount(0);

	pixels = std::vector<std::vector<std::tuple<int,int,int>>> (mandelbrot::imageWidth, std::vector<std::tuple<int,int,int>> (mandelbrot::imageHeight, std::make_tuple(0,0,0)));

	WorkQueue wq;

	wq.start(4);
	auto maxX = mandelbrot::imageWidth;
	for(int y = 0; y < mandelbrot::imageHeight; y++)
	{
		wq.post([=,&amount](){
			{
				mandelbrot::generateSet(0,maxX, y, y + 1, pixels);
				amount++;
				std::cout << "amount complete: " << amount << "/" << mandelbrot::imageHeight << std::endl;;
			};
			if(amount == mandelbrot::imageHeight){
				std::cout << "Work is done! " << amount << std::endl;
				work_done = true;
				std::cout << "Notifying main" << std::endl;
				finished.notify_all();
			}
		});
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
}

void even()
{
	std::atomic<int> amount(0);
	auto n = 4;
	auto minX = 0, maxX = mandelbrot::imageWidth / n, minY = 0, maxY = mandelbrot::imageHeight;

	pixels = std::vector<std::vector<std::tuple<int,int,int>>> (mandelbrot::imageWidth, std::vector<std::tuple<int,int,int>> (mandelbrot::imageHeight, std::make_tuple(0,0,0)));

	WorkQueue wq;

	wq.start(n);
	while(maxX <= mandelbrot::imageWidth)
	{
		wq.post([=,&amount](){
			{
				mandelbrot::generateSet(minX, maxX, 0, maxY, pixels);
				amount++;
				std::cout << "amount complete: " << amount << "/" << n << std::endl;;
			};
			if(amount == n){
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
	while(!(work_done.load()))
	{
		{
			std::unique_lock<std::mutex> l(mutex_wait);
			finished.wait(l, [] {
				return work_done.load();
			});
		};
	}
}

int main(){
	auto trials = 5;
	std::cout << "Please enter the appropriate values for imageWidth, imageHeight, maxN, minR, maxR, minI, maxI\n";
	std::cin >> mandelbrot::imageWidth >> mandelbrot::imageHeight >> mandelbrot::maxN >> mandelbrot::minR >> mandelbrot::maxR >> mandelbrot::minI >> mandelbrot::maxI;

	 //cout << "Please enter the number of trials to find the avg and sd\n";
	 //cin >> trials;
	std::cout << "Chunks" << std::endl;
	chunks();
    // Write generated image to PPM
	std::cout << "Finished generating image, writing to file...\n";
	writeToPpm("chunks",mandelbrot::imageWidth, mandelbrot::imageHeight);
	std::cout << "Finished writing to file.\n";

    work_done = false;
    //std::cout << "Per pixel" << std::endl;
    //perPixel();
    //// Write generated image to PPM
    //std::cout << "Finished generating image, writing to file...\n";
    //writeToPpm("per_pixel",mandelbrot::imageWidth, mandelbrot::imageHeight);
    //std::cout << "Finished writing to file.\n";

    //work_done = false;
	std::cout << "multPixel" << std::endl;
    multPixel();
    // Write generated image to PPM
	std::cout << "Finished generating image, writing to file...\n";
    writeToPpm("mult_pixel",mandelbrot::imageWidth, mandelbrot::imageHeight);
	std::cout << "Finished writing to file.\n";

    work_done = false;
	std::cout << "rows" << std::endl;
    rows();

    // Write generated image to PPM
	std::cout << "Finished generating image, writing to file...\n";
    writeToPpm("rows",mandelbrot::imageWidth, mandelbrot::imageHeight);
	std::cout << "Finished writing to file.\n";

    work_done = true;
	std::cout << "even" << std::endl;
    even();
    // Write generated image to PPM
	std::cout << "Finished generating image, writing to file...\n";
    writeToPpm("even",mandelbrot::imageWidth, mandelbrot::imageHeight);
	std::cout << "Finished writing to file.\n";


	return EXIT_SUCCESS;
}
