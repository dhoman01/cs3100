#include <tuple>
#include <vector>

namespace mandelbrot {
	int imageWidth;
	int imageHeight;
	int maxN;
	double minR;
	double maxR;
	double minI;
	double maxI;
	std::vector<std::tuple<int,int,int>> pixels;
	void generateSet();
	auto findMandelbrot(auto cr, auto ci, auto max_iterations);
	double mapToReal(auto x, auto imageWidth, auto minR, auto maxR);
	double mapToImaginary(auto y, auto imageHeight, auto minI, auto maxI);
	auto getPixels();
	std::tuple<int,int,int> findColor(auto n, auto maxN);

	void generateSet(){
		pixels.clear();
		// Iterate over every pixel
		for (auto  y =0; y < imageWidth; y++) // Rows
		{
			for (auto x = 0; x < imageWidth; x++) // Columns
			{
				// Find the real and imaginary componets of c 
				//     via linear interpolation
				auto cr = mapToReal(x, imageWidth, minR, maxR);
				auto ci = mapToImaginary(y, imageHeight, minI, maxI);

				//  Find the number of iterations in the Mandelbrot formula
				//     using above c.
				auto n = findMandelbrot(cr, ci, maxN);
				std::tuple<int,int,int> color = findColor(n,maxN);
				pixels.push_back(color);
			}
		}
	}

	auto findMandelbrot(auto cr, auto ci, auto max_iterations)
	{
		auto i = 0;
		auto zr = 0.0, zi = 0.0;
		while (i < max_iterations && zr * zr + zi * zi < 4.0)
		{
			auto temp = zr * zr - zi * zi + cr;
			zi = 2.0 * zr * zi + ci;
			zr = temp;
			i++;
		}

		return i;
	}

	// Linear interpolation for the real valued component of the set
	double mapToReal(auto x, auto imageWidth, auto minR, auto maxR)
	{
		auto range = maxR - minR;
		return x * (range / imageWidth) + minR;
	}

	// Linear interpolation for the imaginary component of the set
	double mapToImaginary(auto y, auto imageHeight, auto minI, auto maxI)
	{
		auto range = maxI - minI;
		return y * (range / imageHeight) + minI;
	}

	auto getPixels(){
		return pixels;
	}

	std::tuple<int,int,int> findColor(auto n, auto maxN){
		if(n == maxN){
			return std::make_tuple(0,0,0);
		} else {
			auto t = (double)n / (double) maxN;
			auto r = (int)(8.5*(1-t)*(1-t)*(1-t)*t*255);
			auto g = (int)(15*(1-t)*(1-t)*t*t*255);
			auto b = (int)(9*(1-t)*t*t*t*255);
			return std::make_tuple(r,g,b);
		}
	}
}
