#pragma once
#include <GLM/gtc/noise.hpp>

class Noise
{
public:
	static double getNoise2D(unsigned char countOctaves, double x, double z, double seed, double scale, double persistence = 0.5, double pow = 1.0);
	static double getNoise3D(unsigned char countOctaves, double x, double yy, double z, double seed, double scale, double persistence = 0.5, double pow = 1.0);
};