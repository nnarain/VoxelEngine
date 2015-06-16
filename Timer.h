
#ifndef TIMER_H
#define TIMER_H

namespace engine
{
	class Timer
	{
	public:

		Timer();

		float getElapsed();

	private:

		double _last;

	};
}

#endif
