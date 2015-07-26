
#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

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
		The main shader for the voxel engine
	*/
	class DeferredRenderer : public IRenderer
	{
	public:

		DeferredRenderer(void);
		~DeferredRenderer();

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