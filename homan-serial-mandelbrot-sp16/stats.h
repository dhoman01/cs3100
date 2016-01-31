#include <cmath>
#include <algorithm>

namespace stats {
	std::pair<double, double> getMeanAndSD(auto data){
		auto mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
		auto var = std::accumulate(data.begin(),data.end(),0.0,[mean](auto a, auto data_point){
			return a + std::pow(data_point - mean, 2);
		}) / data.size();
		
		return std::make_pair(mean, std::sqrt(var));
	}
}
	
