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
	void generateSet(auto minX, auto maxX, auto minY, auto maxY, auto& pixels);
	auto findMandelbrot(auto cr, auto ci, auto max_iterations);
	double mapToReal(auto x, auto imageWidth, auto minR, auto maxR);
	double mapToImaginary(auto y, auto imageHeight, auto minI, auto maxI);
	std::tuple<int, int, int> findColor(auto n, auto maxN);
	std::tuple<int,int,int> findColor(int n, int maxN);

	void generateSet(auto minX, auto maxX, auto minY, auto maxY, auto& pixels) {
		bool done = false;
		// Iterate over every pixel
		for (auto  y =minY; y < maxY; y++) // Rows
		{
			for (auto x = minX; x < maxX; x++) // Columns
			{
				// Find the real and imaginary componets of c
				//     via linear interpolation
				auto cr = mapToReal(x, imageWidth, minR, maxR);
				auto ci = mapToImaginary(y, imageHeight, minI, maxI);

				//  Find the number of iterations in the Mandelbrot formula
				//     using above c.
				auto n = findMandelbrot(cr, ci, maxN);
				std::tuple<int,int,int> color = findColor(n,maxN);
				pixels[x][y] = color;
				// std::cout << "pixel: (" << x << ", " << y << ") " << std::get<0>(color) << " " << std::get<1>(color) << " " << std::get<2>(color) << std::endl;
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

	std::tuple<int, int, int> findColor(auto n, auto maxN) {
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
