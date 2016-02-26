#pragma once
#include <random>

static std::random_device device;
inline float randf(float floor = 0.0, float ceil = 1.0){
	std::uniform_real_distribution<float> distribution(floor, ceil);
	return distribution(device);
}

inline int randi(int floor = 0, int ceil = INT_MAX){
	std::uniform_int_distribution<int> distribution(floor, ceil);
	return distribution(device);
}

inline float linearInter(float x, float y, float t){
	return x*(1 - t) + y*t;
}

inline float linearInter(int x, int y, float t){
	return x*(1 - t) + y*t;
}

inline float quadricInter(float v00, float v01, float v10, float v11, float tx, float ty){
	return linearInter(linearInter(v00, v01, tx), linearInter(v10, v11, tx), ty);
}

inline int roundDown(int dividend, int divisor){
	return dividend / divisor*divisor;
}

inline int roundUp(int dividend, int divisor){
	return roundDown(dividend + divisor - 1, divisor);
}