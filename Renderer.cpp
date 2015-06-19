
#include "Renderer.h"

#include <SGL/Util/Exception.h>

#include "GL/glew.h"

#include <iostream>

using namespace engine;
using namespace sgl;

Renderer::Renderer(void)
{
}

void Renderer::init()
{
	try
	{
		_chunkShader.load(ShaderProgram::Type::VERTEX,   "");
		_chunkShader.load(ShaderProgram::Type::FRAGMENT, "");
		_chunkShader.addAttribute("vPosition", 3);
		_chunkShader.addAttribute("vNormal",   3);
		_chunkShader.addAttribute("vTexCoord", 2);

		_chunkShader.link();

	}
	catch (Exception& e)
	{
		std::cout << "Error initializing the Renderer" << std::endl;
		std::cout << e.what() << std::endl;
	}

	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::begin()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_chunkShader.begin();
}

void Renderer::render(ChunkManager& chunkManager)
{
}

void Renderer::end()
{
	_chunkShader.end();
}

Renderer::~Renderer()
{
}
