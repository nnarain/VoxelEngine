
#ifndef GBUFFER_H
#define GBUFFER_H

#include <SGL/GL/FrameBuffer.h>
#include <SGL/GL/Texture.h>
#include <SGL/GL/RenderBuffer.h>

#include <map>
#include <string>

namespace engine
{
	class GBuffer
	{
	public:

		GBuffer();
		~GBuffer();

		void init(int width, int height);

		void bindForWriting();
		void bindForReading();

		void unbind();

		sgl::Texture& getNormalTexture();
		sgl::Texture& getDiffuseTexture();
		sgl::Texture& getColorTexture();

		sgl::Texture& getTexture(const std::string& key);

	private:
		sgl::FrameBuffer _fbo;

		sgl::Texture _normalTexture;
		sgl::Texture _diffuseTexture;
		sgl::Texture _colorTexture;

		sgl::RenderBuffer _depthBuffer;

		std::map<std::string, sgl::Texture*> _textureMap;

	private:

		void initTexture(sgl::Texture& texture, int width, int height);
	};
}

#endif
