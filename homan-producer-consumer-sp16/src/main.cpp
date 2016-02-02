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

using namespace std;

vector<vector<tuple<int,int,int>>> pixels;
mutex mutex_wait;
mutex mutex_pixel;
condition_variable finished;
atomic<bool> work_done(false);

// Write the image to a PPM
void writeToPpm(string name, auto imageWidth, auto imageHeight){
	// Open the output file and write the PPM header...
	ofstream fout(name.append("_output_image.ppm"));
	fout << "P3" << endl;                                                        // "Magic Number" - PPM file
	fout << imageWidth << " " << imageHeight << endl;    // Dimensions
	fout << "255" << endl;                                                      // Maximum value of a pixel R,G,B value...

	//this_thread::sleep_for(chrono::seconds(5));

	// Write to pixels
	for( auto y = 0; y < imageHeight; y++){
		for(auto x = 0; x < imageWidth; x++){
			//cout << "Pixel: " << x << " " << y << endl;
			lock_guard<mutex> l(mutex_pixel);
			auto color = pixels[x][y];                 // Color stored as tuple<Red, Green, Blue>
			fout << " " << get<0>(color) << " " << get<1>(color) << " " << get<2>(color) << " ";
		}
		fout << endl;
	}

	fout.close();
	return;
}

void chunks()
{
	atomic<int> amount(0);
	auto n = 32;

	auto minX = 0;
	auto maxX = mandelbrot::imageWidth / n;
	auto minY = 0;
	auto maxY = mandelbrot::imageHeight;

	{
		pixels = vector<vector<tuple<int,int,int>>> (mandelbrot::imageWidth, vector<tuple<int,int,int>> (mandelbrot::imageHeight, make_tuple(0,0,0)));
	};

	WorkQueue wq;

	wq.start(4);

	while(maxX <= mandelbrot::imageWidth)
	{
		wq.post([=](){
			{
				mandelbrot::generateSet(minX, maxX, 0, maxY, pixels);
				amount++;
				cout << "amount complete: " << amount << "/" << n << endl;;
			};
			if(amount == n){
				cout << "Work is done!" << endl;
				work_done = true;
				cout << "Notifying main" << endl;
				finished.notify_all();
			}
		});
		minX = maxX;
		maxX += mandelbrot::imageWidth / n;
	}

	cout << "All the work has been added" << endl;
	while(!(work_done.load()))
	{
		{
			unique_lock<mutex> l(mutex_wait);
			finished.wait(l, [] {
				return work_done.load();
			});
		};
	}
}

void perPixel()
{
	pixels = vector<vector<tuple<int,int,int>>> (mandelbrot::imageWidth, vector<tuple<int,int,int>> (mandelbrot::imageHeight, make_tuple(0,0,0)));

	WorkQueue wq;

	wq.start(4);
    auto total = mandelbrot::imageWidth * mandelbrot::imageHeight;
	for(int x = 0; x < mandelbrot::imageWidth; x++)
	{
		for(int y = 0; y < mandelbrot::imageHeight; y++)
		{
			wq.post([=](){
                mandelbrot::generateSet(x, x + 1, y, y + 1, pixels);
                amount++;
                cout << "amount complete: " << amount << "/" << total << endl;
				if(amount == total)
				{
					cout << "Work is done!" << endl;
					work_done = true;
					cout << "Notifying main" << endl;
					finished.notify_all();
				}
			});
		}
	}

	cout << "All the work has been added" << endl;
	while(!(work_done.load()))
	{
		{
			unique_lock<mutex> l(mutex_wait);
			finished.wait(l, [] {
				return work_done.load();
			});
		};
	}
}

void multPixel()
{
	auto n = 32;

	{
		pixels = vector<vector<tuple<int,int,int>>> (mandelbrot::imageWidth, vector<tuple<int,int,int>> (mandelbrot::imageHeight, make_tuple(0,0,0)));
	};

	WorkQueue wq;

	wq.start(4);

	for(int y = 0; y < mandelbrot::imageHeight; y++)
	{
		auto minX = 0;
		auto maxX = mandelbrot::imageWidth / n;
		while(maxX <= mandelbrot::imageWidth)
		{
			wq.post([minX,maxX,y,n](){
				{
					mandelbrot::generateSet(minX, maxX, y, y + 1, pixels);
					amount++;
					cout << "amount complete: " << amount << "/" << n * mandelbrot::imageHeight << endl;;
				};
				if(amount == n * mandelbrot::imageHeight){
					cout << "Work is done!" << endl;
					work_done = true;
					cout << "Notifying main" << endl;
					finished.notify_all();
				}
			});

			minX = maxX;
			maxX += mandelbrot::imageWidth / n;
		}
	}

	cout << "All the work has been added" << endl;
	while(!(work_done.load()))
	{
		{
			unique_lock<mutex> l(mutex_wait);
			finished.wait(l, [] {
				return work_done.load();
			});
		};
	}
}

void rows()
{
	{
		pixels = vector<vector<tuple<int,int,int>>> (mandelbrot::imageWidth, vector<tuple<int,int,int>> (mandelbrot::imageHeight, make_tuple(0,0,0)));
	};

	WorkQueue wq;

	wq.start(4);
	auto maxX = mandelbrot::imageWidth;
	for(int y = 0; y < mandelbrot::imageHeight; y++)
	{
		wq.post([=](){
			{
				mandelbrot::generateSet(0,maxX, y, y + 1, pixels);
				amount++;
				cout << "amount complete: " << amount << "/" << mandelbrot::imageHeight << endl;;
			};
			if(amount == mandelbrot::imageHeight){
				cout << "Work is done! " << amount << endl;
				work_done = true;
				cout << "Notifying main" << endl;
				finished.notify_all();
			}
		});
	}

	cout << "All the work has been added" << endl;
	while(!(work_done.load()))
	{
		{
			unique_lock<mutex> l(mutex_wait);
			finished.wait(l, [] {
				return work_done.load();
			});
		};
	}
}

void even()
{
	auto n = 4;
	auto minX = 0, maxX = mandelbrot::imageWidth / n, minY = 0, maxY = mandelbrot::imageHeight;

	{
		pixels = vector<vector<tuple<int,int,int>>> (mandelbrot::imageWidth, vector<tuple<int,int,int>> (mandelbrot::imageHeight, make_tuple(0,0,0)));
	};

	WorkQueue wq;

	wq.start(n);
	while(maxX <= mandelbrot::imageWidth)
	{
		wq.post([=](){
			{
				mandelbrot::generateSet(minX, maxX, 0, maxY, pixels);
				amount++;
				cout << "amount complete: " << amount << "/" << n << endl;;
			};
			if(amount == n){
				cout << "Work is done!" << endl;
				work_done = true;
				cout << "Notifying main" << endl;
				finished.notify_all();
			}
		});
		minX = maxX;
		maxX += mandelbrot::imageWidth / n;
	}

	cout << "All the work has been added" << endl;
	while(!(work_done.load()))
	{
		{
			unique_lock<mutex> l(mutex_wait);
			finished.wait(l, [] {
				return work_done.load();
			});
		};
	}
}

int main(){
	auto trials = 5;
	cout << "Please enter the appropriate values for imageWidth, imageHeight, maxN, minR, maxR, minI, maxI\n";
	cin >> mandelbrot::imageWidth >> mandelbrot::imageHeight >> mandelbrot::maxN >> mandelbrot::minR >> mandelbrot::maxR >> mandelbrot::minI >> mandelbrot::maxI;

//	 cout << "Please enter the number of trials to find the avg and sd\n";
//	 cin >> trials;
//	cout << "Chunks" << endl;
//	chunks();
//    // Write generated image to PPM
//	cout << "Finished generating image, writing to file...\n";
//	writeToPpm("chunks",mandelbrot::imageWidth, mandelbrot::imageHeight);
//	cout << "Finished writing to file.\n";
//    amount = 0;
//
//    work_done = false;
    cout << "Per pixel" << endl;
    perPixel();
    // Write generated image to PPM
    cout << "Finished generating image, writing to file...\n";
    writeToPpm("per_pixel",mandelbrot::imageWidth, mandelbrot::imageHeight);
    cout << "Finished writing to file.\n";
    amount = 0;
    work_done = false;
//    cout << "multPixel" << endl;
//    multPixel();
//    // Write generated image to PPM
//    cout << "Finished generating image, writing to file...\n";
//    writeToPpm("mult_pixel",mandelbrot::imageWidth, mandelbrot::imageHeight);
//    cout << "Finished writing to file.\n";
//    amount = 0;
//    work_done = false;
//    cout << "rows" << endl;
//    rows();
//
//    // Write generated image to PPM
//    cout << "Finished generating image, writing to file...\n";
//    writeToPpm("rows",mandelbrot::imageWidth, mandelbrot::imageHeight);
//    cout << "Finished writing to file.\n";
//
//    amount = 0;
//    work_done = true;
//    cout << "even" << endl;
//    even();
//    // Write generated image to PPM
//    cout << "Finished generating image, writing to file...\n";
//    writeToPpm("even",mandelbrot::imageWidth, mandelbrot::imageHeight);
//    cout << "Finished writing to file.\n";


	return EXIT_SUCCESS;
}
