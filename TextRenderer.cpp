
#include "TextRenderer.h"

#include <SGL/Util/Exception.h>

#include <SGL/Util/GLSL/GLSL_Position_TexCoord_Color.h>

using namespace sgl;

namespace engine
{
	TextRenderer::TextRenderer()
	{
	}

	void TextRenderer::begin()
	{
		_batch.begin(&_textShader);
	}

	void TextRenderer::draw(sgl::Text& text)
	{
		draw(text, false, false);
	}

	void TextRenderer::draw(sgl::Text& text, bool flipH, bool flipV)
	{
		text.draw(_batch, flipH, flipV);
	}

	void TextRenderer::end()
	{
		_batch.end();
	}

	void TextRenderer::init()
	{
		try
		{
			loadshader::position_texCoord_color(_textShader);
		}
		catch (Exception& e)
		{
			throw std::exception(e.what());
		}
	}

	TextRenderer::~TextRenderer()
	{
	}
}
