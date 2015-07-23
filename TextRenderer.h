
#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <SGL/Graphics/SpriteBatch.h>
#include <SGL/GL/ShaderProgram.h>
#include <SGL/Graphics/Text.h>

namespace engine
{
	class TextRenderer
	{
	public:

		TextRenderer();
		~TextRenderer();

		void init();

		void begin();

		void draw(sgl::Text& text);
		void draw(sgl::Text& text, bool flipH, bool flipV);

		void end();

	private:
		sgl::SpriteBatch   _batch;
		sgl::ShaderProgram _textShader;
	};
}

#endif
