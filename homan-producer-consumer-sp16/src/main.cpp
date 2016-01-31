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
#include "mandelbrot.hpp"
#include "stats.hpp"
#include "timer.hpp"
#include "WorkQueue.cpp"

using namespace std;

auto n = 4;
vector<vector<tuple<int,int,int>>> pixels;
mutex mutex_wait;
mutex mutex_pixel;
condition_variable finished;
atomic<bool> work_done(false);

// Write the image to a PPM
void writeToPpm(auto imageWidth, auto imageHeight){
	// Open the output file and write the PPM header...
	ofstream fout("output_image.ppm");                               
	fout << "P3" << endl;                                                        // "Magic Number" - PPM file
	fout << imageWidth << " " << imageHeight << endl;    // Dimensions
	fout << "255" << endl;                                                      // Maximum value of a pixel R,G,B value...
	
	// Write to pixels
	for( auto y = 0; y < imageHeight; y++){
		for(auto x = 0; x < imageWidth; x++){
			lock_guard<mutex> l(mutex_pixel);
			auto color = pixels[x][y];                 // Color stored as tuple<Red, Green, Blue>
			fout << " " << get<0>(color) << " " << get<1>(color) << " " << get<2>(color) << " ";
		}
		fout << endl;
	}

	fout.close();
	return;
}

void placePixel(auto pixel){
	{
		lock_guard<mutex> l (mutex_pixel);
		pixels[get<0>(pixel)][get<1>(pixel)] = get<2>(pixel);
	};
	if(get<0>(pixel) >= mandelbrot::imageWidth - 1 && get<1>(pixel) >= mandelbrot::imageHeight -1 )
	{ 
		cout << "Setting work_done to true" << endl;
		work_done = true;
		cout << "Notifying all" << endl;
		finished.notify_all();
	}
}

void rows()
{
	auto n = 32;
	
	auto minW = 0;
	auto maxW = mandelbrot::imageWidth / n;
	auto minY = 0;
	auto maxY = mandelbrot::imageHeight;
	
	{
		lock_guard<mutex> l(mutex_pixel);	
		pixels = vector<vector<tuple<int,int,int>>> (mandelbrot::imageWidth, vector<tuple<int,int,int>> (mandelbrot::imageHeight, make_tuple(0,0,0)));
	};
	
	WorkQueue wq;
	
	wq.start(4);
	
	while(maxW <= mandelbrot::imageWidth)
	{
		wq.post([minW,maxW,minY,maxY](){
			auto pixel = mandelbrot::generateSet(minW, maxW, 0, maxY);
			cout << "Placing pixel: " << get<0>(pixel) << " " << get<1>(pixel) << endl;
			placePixel(pixel);
		});
		minW = maxW;
		maxW += mandelbrot::imageWidth / n;
	}
	
	cout << "All the work has been added" << endl;
	while(!(work_done.load()))
	{
		cout << "work_done " << work_done.load() << endl;
		{
			auto done = work_done.load();
			unique_lock<mutex> l(mutex_wait);
			cout << "calling wait on main" << endl;
			finished.wait(l, [done] {return done;});
		};
		cout << "main notified" << endl;
	}
}

int main(){
	auto trials = 5;
	cout << "Please enter the appropriate values for imageWidth, imageHeight, maxN, minR, maxR, minI, maxI\n";
	cin >> mandelbrot::imageWidth >> mandelbrot::imageHeight >> mandelbrot::maxN >> mandelbrot::minR >> mandelbrot::maxR >> mandelbrot::minI >> mandelbrot::maxI;
	
	// cout << "Please enter the number of trials to find the avg and sd\n";
	// cin >> trials;
	
	rows();	
	
	// Write generated image to PPM
	cout << "Finished generating image, writing to file...\n";
	writeToPpm(mandelbrot::imageWidth, mandelbrot::imageHeight);
	cout << "Finished writing to file.\n";
	
	return EXIT_SUCCESS;
}
