// Author: Dustin Homan
// A#: A02177041
// Assignment-two (Threaded Mandelbrot)
// This is the main driver and contains the
//     function that writes the image to a PPM
//     and executes the multiple threads
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <tuple>
#include <thread>
#include "mandelbrot.h"
#include "stats.h"
#include "timer.h"

using namespace std;

auto n = 4;
vector<vector<tuple<int,int,int>>> pixels;

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
			auto color = pixels[x][y];                 // Color stored as tuple<Red, Green, Blue>
			fout << " " << get<0>(color) << " " << get<1>(color) << " " << get<2>(color) << " ";
		}
		fout << endl;
	}

	fout.close();
	return;
}

void threaded_mandelbrot(){
	vector<thread> threads;
	
	auto minX = 0, maxX = mandelbrot::imageWidth / n, minY = 0, maxY = mandelbrot::imageHeight;
	
	for(auto i = 0; i < n; i++){
		threads.push_back(move(thread([minX, maxX, minY, maxY](){
			mandelbrot::generateSet(minX, maxX, minY, maxY, pixels);
		})));
		minX = maxX;
		maxX += mandelbrot::imageWidth / n;
	}
	
	// Join on each thread
	for_each(threads.begin(), threads.end(), [](auto& t){ t.join(); });
}

int main(){
	auto trials = 5, maxT = 8;
	cout << "Please enter the appropriate values for imageWidth, imageHeight, maxN, minR, maxR, minI, maxI\n";
	cin >> mandelbrot::imageWidth >> mandelbrot::imageHeight >> mandelbrot::maxN >> mandelbrot::minR >> mandelbrot::maxR >> mandelbrot::minI >> mandelbrot::maxI;
	
	cout << "Please enter the number of trials to find the avg and sd\n";
	cin >> trials;
	
	cout << "Please enter the max number of threads 1-8\n";
	cin >> maxT;
	
	// Map for comparing running times
	vector<pair<int,double>> thread_avg_pairs;
	
	// File for storing timing data
	ofstream fout;
	fout.open("time_results.txt");

	for(n = 1; n <= maxT; n++){
		pixels = vector<vector<tuple<int,int,int>>> (mandelbrot::imageWidth, vector<tuple<int,int,int>> (mandelbrot::imageHeight, make_tuple(0,0,0)));
	
		// Vector to store the trials' running times
		vector<double> running_times;
	
		// Run the trials
		for(auto i = 0; i < trials; i++){
			chrono::duration<double> time_span = timer::timeFunction<decltype(threaded_mandelbrot)>(threaded_mandelbrot);

			// Store the running time for further calculations
			running_times.push_back(time_span.count());
		}
	
		// Calculate the average and standard deviation of the trials
		pair<double,double> avg_sd = stats::getMeanAndSD(running_times);
		thread_avg_pairs.push_back(make_pair(n,avg_sd.first));		
	
		fout << "=======================================\n";
		fout << "Image stats: imageWidth, imageHeight, maxN, minR, maxR, minI, maxI\n";
		fout << mandelbrot::imageWidth << ", " << mandelbrot::imageHeight << ", " << mandelbrot::maxN;
		fout << ", " << mandelbrot::minR << ", " << mandelbrot::maxR << ", " << mandelbrot::minI << ", " << mandelbrot::maxI << endl;
		fout << "# of trials: " << trials << endl;
		fout << "# of threads: " << n << endl;
		fout << "Threaded average running time: " << avg_sd.first << "\n";
		fout << "Threaded standard deviation: " << avg_sd.second << "\n";
		fout << "=======================================\n\n";
	}
	double org_avg = thread_avg_pairs[0].second;
	cout << "org_avg=" << org_avg << endl;
	for_each(thread_avg_pairs.begin(), thread_avg_pairs.end(), [&fout,org_avg](auto& pair){
		fout << "=======================================\n";
		fout << "Comparing 1 thread to " << pair.first << " threads" << endl;
		double S =  stats::getSpeedUp(org_avg, pair.second);
		fout << "Speed up: " << S << endl;
		double E = stats::getEfficiency(S, pair.first);
		fout << "Efficiency: " << E << endl;
		fout << "=======================================\n\n";
	});
	
	fout.close();
	
	// Write generated image to PPM
	cout << "Finished generating image, writing to file...\n";
	writeToPpm(mandelbrot::imageWidth, mandelbrot::imageHeight);
	cout << "Finished writing to file.\n";
	
	return EXIT_SUCCESS;
}
