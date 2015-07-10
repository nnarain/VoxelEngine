
#ifndef RENDERER_H
#define RENDERER_H

#include "IRenderer.h"

#include "ChunkManager.h"
#include "TextureManager.h"

#include "GBuffer.h"

#include <SGL/GL/ShaderProgram.h>
#include <SGL/GL/Texture.h>
#include <SGL/GL/Mesh.h>

#include <SGL/Math/Matrix4.h>

namespace engine
{
	/**
	*/
	class Renderer : public IRenderer
	{
	public:

		Renderer(void);
		~Renderer();

		void init();

		void begin();
		void render(ChunkManager& chunkManager, sgl::Matrix4& VP);
		void end();

	private:
		sgl::ShaderProgram _geometryPass;
		sgl::ShaderProgram _lightPass;

		GBuffer            _gBuffer;

		sgl::Mesh          _screenMesh;

	private:

		void initScreenMesh(sgl::Mesh& mesh);

	};
}

#endif