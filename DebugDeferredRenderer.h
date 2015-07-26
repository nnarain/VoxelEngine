
#ifndef DEBUGDEFERREDRENDERER_H
#define DEBUGDEFERREDRENDERER_H

#include "IRenderer.h"

#include "GBuffer.h"

#include <SGL/GL/ShaderProgram.h>
#include <SGL/GL/Texture.h>
#include <SGL/GL/Mesh.h>

#include <SGL/Math/Matrix4.h>

namespace engine
{
	/**
		Shader to select which textures from the GBuffer to display
	*/
	class DebugDeferredRenderer : public IRenderer
	{
	public:

		DebugDeferredRenderer(void);
		~DebugDeferredRenderer();

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
