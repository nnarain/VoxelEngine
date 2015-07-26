
#ifndef NOISE_H
#define NOISE_H

#include <vector>

namespace engine
{
	namespace noise
	{
		/**
			Random value noise class
		*/
		class Noise
		{
		public:

			Noise(int w, int h);

			void generate(int octaveCount);

			float at(int x, int y);

		private:

			int _width;
			int _height;

			std::vector<float> _noise;

			/* Private Functions */

			void generateWhiteNoise(std::vector<float>& arr);
			void generateSmoothNoise(std::vector<float>& out, std::vector<float>& baseNoise, int octave);

			void set(std::vector<float>& noise, int x, int y, float value);
			float& get(std::vector<float>& arr, int x, int y);

		};
	}
}

#endif
