
#include "IRenderer.h"

using namespace engine;

IRenderer::IRenderer()
{
}

TextureManager& IRenderer::getTextureManager()
{
	return _textureManager;
}
