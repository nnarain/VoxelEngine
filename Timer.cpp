
#include "Timer.h"

#include <glfw/glfw3.h>

using namespace engine;

Timer::Timer()
{
	_last = glfwGetTime();
}

float Timer::getElapsed()
{
	double current = glfwGetTime();
	float delta = (float)(current - _last);

	_last = current;

	return delta;
}