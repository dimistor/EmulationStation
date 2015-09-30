#pragma once

#include <Eigen/Dense>

class Animation
{
public:
	virtual int getDuration() const = 0;
	virtual void apply(float t) = 0;
};


// useful helper/interpolation functions
inline float clamp(float min, float max, float val)
{
	if(val < min)
		val = min;
	else if(val > max)
		val = max;

	return val;
}

//http://en.wikipedia.org/wiki/Smoothstep
inline float smoothStep(float edge0, float edge1, float x)
{
	// Scale, and clamp x to 0..1 range
	x = clamp(0, 1, (x - edge0)/(edge1 - edge0));

	// Evaluate polynomial
	return x*x*x*(x*(x*6 - 15) + 10);
}

template<typename T>
T lerp(const T& start, const T& end, float t)
{
	if(t <= 0.0f)
		return start;
	if(t >= 1.0f)
		return end;

	return (start * (1 - t) + end * t);
}

inline unsigned int lerpColor(unsigned int color0, unsigned int color1, float t)
{
	if(t <= 0.0f)
		return color0;
	if(t >= 1.0f)
		return color1;

	unsigned int den = 0x00FFFFFF;
	unsigned int num0 = den - den * t;
	unsigned int num1 = den * t;
	unsigned int result = 0;

	unsigned int r = ((((color0 & 0xff000000) >> 24) * num0 + ((color1 & 0xff000000) >> 24) * num1) / den) << 24;
	unsigned int g = ((((color0 & 0x00ff0000) >> 16) * num0 + ((color1 & 0x00ff0000) >> 16) * num1) / den) << 16;
	unsigned int b = ((((color0 & 0x0000ff00) >> 8 ) * num0 + ((color1 & 0x0000ff00) >> 8 ) * num1) / den) << 8;
	unsigned int a = ((((color0 & 0x000000ff)      ) * num0 + ((color1 & 0x000000ff)      ) * num1) / den);

	return r | g | b | a;
}
