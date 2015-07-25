
#ifndef VOXELENGINE_H
#define VOXELENGINE_H

#include "ChunkManager.h"
#include "FPSCamera.h"
#include "Window.h"
#include "IRenderer.h"
#include "ResourceManager.h"
#include "TextRenderer.h"
#include "Logger.h"
#include "CommandLine.h"

#include <SGL/Util/DebugRenderer.h>
#include <SGL/Graphics/SpriteBatch.h>

#include <set>
#include <memory>

namespace engine
{
	class VoxelEngine
	{
	public:

		~VoxelEngine();
		
		void update();
		void render();

		void addChunkManager(ChunkManager* manager);

		void updateCamera(float delta);

		void createWindow(const char * title, int width, int height);

		void loadTexture(const char *textureName);
		void loadAtlas(const char *atlasName);
		void loadFont(const char *fontname, int cols, int rows, bool flip);

		FPSCamera* getCamera();

		gui::Window* getWindow();

		IRenderer& getRenderer();

		void setRenderer(unsigned int idx);
		void setRenderOption(const char *key, const char *value);

		ResourceManager& getResources();

		util::Logger& getLogger();

		gui::CommandLine* getCommandLine();

		/**
			Return the instance of the voxel engine
		*/
		static VoxelEngine* getEngine();

	private:
		/* Block creation of an engine from outside this class */
		VoxelEngine();
		VoxelEngine(const VoxelEngine&);

		//
		std::set<ChunkManager*> _chunkManagers;
		bool _updateChunks;

		//
		std::unique_ptr<gui::Window> _window;

		//
		IRenderer* _renderer;

		std::vector<IRenderer*> _renderers;

		// 
		ResourceManager _resources;

		//
		FPSCamera _camera;

		//
		std::unique_ptr<gui::CommandLine> _commandLine;

		//
		std::unique_ptr<TextRenderer> _textRenderer;

		//
		util::Logger _logger;

	private:

		void updateChunkManagersVisibility(sgl::Frustum& frustum);

		void initializeContext();

		void allocateRenderers();

		void addCommandLineFunctions();
	};
}

#endif
