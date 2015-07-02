
#include "Renderer.h"
#include "DefaultShaders.h"

#include "GL/glew.h"

#include <SGL/Util/Exception.h>
#include <SGL/Math/Vector3.h>

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
		_chunkShader.load(ShaderProgram::Type::VERTEX,   GLSL_VERTEX_NORMAL_TEXTURE_VERT);
		_chunkShader.load(ShaderProgram::Type::FRAGMENT, GLSL_VERTEX_NORMAL_TEXTURE_FRAG);
		_chunkShader.addAttribute("vPosition", 3);
		_chunkShader.addAttribute("vNormal",   3);
		_chunkShader.addAttribute("vTexCoord", 2);

		_chunkShader.link();
	}
	catch (Exception& e)
	{
		std::cout << "Error initializing the Renderer" << std::endl;
		std::cout << e.what() << std::endl;

		exit(1);
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

void Renderer::render(ChunkManager& chunkManager, Matrix4& VP)
{
	Matrix4& M = chunkManager.getWorldTransform();
	Matrix4 MVP = VP * M;

	_chunkShader["MVP"].set(MVP);

	std::string atlasName = chunkManager.getAtlasName();

	Texture& texture = _textureManager.getTexture(atlasName);
	texture.bind(Texture::Unit::T0);

	_chunkShader["blockTexture"].set(texture);

	chunkManager.render();

	texture.unbind();
}

void Renderer::end()
{
	_chunkShader.end();
}

TextureManager& Renderer::getTextureManager()
{
	return _textureManager;
}

Renderer::~Renderer()
{
}
