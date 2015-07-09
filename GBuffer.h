
#ifndef GBUFFER_H
#define GBUFFER_H

#include <SGL/GL/FrameBuffer.h>
#include <SGL/GL/Texture.h>
#include <SGL/GL/RenderBuffer.h>

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

	private:
		sgl::FrameBuffer _fbo;

		sgl::Texture _normalTexture;
		sgl::Texture _diffuseTexture;

		sgl::RenderBuffer _depthBuffer;

	private:

		void initTexture(sgl::Texture& texture, int width, int height);
	};
}

#endif
