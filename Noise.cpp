
#include "Noise.h"

using namespace engine;
using namespace engine::noise;

#include <SGL/Math/MathUtil.h>

#include <cassert>
#include <random>

Noise::Noise(int w, int h) :
	_width(w),
	_height(h),
	_noise(w * h, 0)
{
}

void Noise::generate(int octaveCount)
{
	std::vector<std::vector<float>> smoothNoise(octaveCount);
	std::vector<std::vector<float>>::iterator iter;
	for (iter = smoothNoise.begin(); iter != smoothNoise.end(); ++iter)
	{
		std::vector<float>& v = (*iter);
		v.resize(_width * _height);
	}

	std::vector<float> baseNoise(_width * _height);
	generateWhiteNoise(baseNoise);

	int octave;
	for (octave = 0; octave < octaveCount; ++octave)
	{
		generateSmoothNoise(smoothNoise[octave], baseNoise, octave);
	}

	float amplitude = 1.0f;
	float totalAmplitude = 0.0f;
	float persistance = 0.5f;

	// blend noise
	for (octave = octaveCount - 1; octave >= 0; --octave)
	{
		amplitude *= persistance;
		totalAmplitude += amplitude;

		int i, j;
		for (i = 0; i < _width; ++i)
		{
			for (j = 0; j < _height; ++j)
			{
				float &noise = get(_noise, i, j);
				noise += get(smoothNoise[octave], i, j) * amplitude;
			}
		}
	}

	// normalize
	int i, j;
	for (i = 0; i < _width; ++i)
	{
		for (j = 0; j < _height; ++j)
		{
			float &noise = get(_noise, i, j);
			noise /= totalAmplitude;
		}
	}
}

void Noise::generateSmoothNoise(std::vector<float>& out, std::vector<float>& baseNoise, int octave)
{
	int   period = 1 << octave;
	float frequency = 1.0f / (float)period;

	int i, j;
	for (i = 0; i < _width; ++i)
	{
		int samplei0 = (i / period) * period;
		int samplei1 = (samplei0 + period) % _width;
		float hBlend = (float)(i - samplei0) * frequency;

		for (j = 0; j < _height; ++j)
		{
			int samplej0 = (j / period) * period;
			int samplej1 = (samplej0 + period) % _height;
			float vBlend = (float)(j - samplej0) * frequency;

			float top = sgl::math::lerp<float>(get(baseNoise, samplei0, samplej0), get(baseNoise, samplei1, samplej0), hBlend);
			float bottom = sgl::math::lerp<float>(get(baseNoise, samplei0, samplej1), get(baseNoise, samplei1, samplej1), hBlend);

			set(out, i, j, sgl::math::lerp<float>(top, bottom, vBlend));
		}
	}
}

void Noise::generateWhiteNoise(std::vector<float>& arr)
{
	int i, j;
	for (i = 0; i < _width; ++i)
	{
		for (j = 0; j < _height; ++j)
		{
			set(arr, i, j, (float)(rand() % 2));
		}
	}
}

void Noise::set(std::vector<float>& arr, int x, int y, float value)
{
	arr[(x * _width) + y] = value;
}

float& Noise::get(std::vector<float>& arr, int x, int y)
{
	return arr[(x * _width) + y];
}

float Noise::at(int x, int y)
{
	assert(x < _width && y < _height && "Value out of range");

	return _noise[(x * _width) + y];
}

