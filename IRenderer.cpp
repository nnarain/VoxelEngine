
#include "IRenderer.h"

using namespace engine;

IRenderer::IRenderer() : initialized(false)
{
}

bool IRenderer::isInitialized()
{
	return initialized;
}

void IRenderer::setRenderOption(const std::string& key, const std::string& value)
{
	renderOptions[key] = value;
}

std::string IRenderer::getRenderOption(const std::string& key)
{
	return renderOptions[key];
}