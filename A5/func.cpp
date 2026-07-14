#include <algorithm>
#include <vector>
#include <cmath>


inline int max1(std::vector<int> p) {
	return std::max(p[0], p[1]);
}
inline int min1(std::vector<int> p) {
	return std::min(p[0], p[1]);
}

inline int plus1(std::vector<int> p) {
	return p[0] + p[1];
}
inline int minus1(std::vector<int> p) {
	return p[0] - p[1];
}
inline int multiply1(std::vector<int> p) {
	return p[0] * p[1];
} 
inline int divide1(std::vector<int> p) {
	return p[0] / p[1];
}

inline int pow1(std::vector<int> p) {
	return std::pow(p[0], p[1]);
}

inline int abs1(std::vector<int> p) {
	return std::abs(p[0]);
}