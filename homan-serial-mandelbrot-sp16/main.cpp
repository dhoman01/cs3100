// Author: Dustin Homan
// A#: A02177041
// Assignment-One (Serial Mandelbrot)
// This is the main driver and contains the
//     function that writes the image to a PPM

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <tuple>
#include "stats.h"                      // Header containing stats functions
#include "mandelbrot.h"          // Header containing functions for the Mandelbrot Set
#include "timer.h"                     // Header containing timing functions


using namespace std;

// Write the image to a PPM
void writeToPpm(auto pixels, auto imageWidth, auto imageHeight){
	// Open the output file and write the PPM header...
	ofstream fout("output_image.ppm");                               
	fout << "P3" << endl;                                                        // "Magic Number" - PPM file
	fout << imageWidth << " " << imageHeight << endl;    // Dimensions
	fout << "255" << endl;                                                      // Maximum value of a pixel R,G,B value...

	// Write the pixels
	for(auto i = 0; i < pixels.size(); i++){
		auto color = pixels.at(i);                 // Color stored as tuple<Red, Green, Blue>
        	fout << " " << get<0>(color) << " " << get<1>(color) << " " << get<2>(color) << " ";
        	// Add a newline character at the end of each row
		if(i % imageWidth == 0){
			fout << endl;
		}
	}

	fout.close();
	return;
}

int main()
{
	// Get values from user
	auto imageWidth = 512, imageHeight = 512, maxN = 100, trials = 50;
	auto minR = -2.0, maxR = 2.0, minI = -2.0, maxI = 2.0;

	cout << "Please enter the appropriate values for imageWidth, imageHeight, maxN, minR, maxR, minI, maxI\n";
	cin >> mandelbrot::imageWidth >> mandelbrot::imageHeight >> mandelbrot::maxN >> mandelbrot::minR >> mandelbrot::maxR >> mandelbrot::minI >> mandelbrot::maxI;

	cout << "Please enter the number of trials to find the avg and sd\n";
	cin >> trials;

	// Vector to store the trials' running times
	vector<double> running_times;
	
	// Run the trials
	for(auto i = 0; i < trials; i++){
		chrono::duration<double> time_span = timer::timeFunction<decltype(mandelbrot::generateSet)>(mandelbrot::generateSet);

		// Store the running time for further calculations
		running_times.push_back(time_span.count());
	}

	// Calculate the average and standard deviation of the trials
	pair<double,double> avg_sd = stats::getMeanAndSD(running_times);

	cout << "\nAverage running times: " << avg_sd.first << "\n";
	cout << "Standard Deviation: " << avg_sd.second << "\n";

	// Write the pixels out to ppm file
	writeToPpm(mandelbrot::getPixels(), mandelbrot::imageWidth, mandelbrot::imageHeight);

	cout << "\nFinished!" << endl;

	return 0;
}
