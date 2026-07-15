#include <iostream> 
#include <algorithm>
#include <vector>
#include <cmath>


inline float max1(std::vector<float> p) {
	return std::max(p[0], p[1]);
}
inline float min1(std::vector<float> p) {
	return std::min(p[0], p[1]);
}

inline float plus1(std::vector<float> p) {
	return p[0] + p[1];
}
inline float minus1(std::vector<float> p) {
	return p[0] - p[1];
}
inline float multiply1(std::vector<float> p) {
	return p[0] * p[1];
} 
inline float divide1(std::vector<float> p) {
	if (p[1] == 0) {
		std::cout << "Zero division!\n";
		return 0;
	}
	return p[0] / p[1];
}

inline float pow1(std::vector<float> p) {
	return std::pow(p[0], p[1]);
}

inline float abs1(std::vector<float> p) {
	return std::abs(p[0]);
}