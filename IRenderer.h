
#ifndef IRENDERER_H
#define IRENDERER_H

#include "ChunkManager.h"
#include "TextureManager.h"

#include <SGL/Math/Matrix4.h>

#include <map>
#include <string>


namespace engine
{
	class IRenderer
	{
	public:

		IRenderer();

		virtual void init()                                               = 0;

		virtual void begin()                                              = 0;
		virtual void render(ChunkManager& chunkManager, sgl::Matrix4& VP) = 0;
		virtual void end()                                                = 0;

		bool isInitialized();

		void setRenderOption(const std::string& key, const std::string& value);
		std::string getRenderOption(const std::string& key);

	protected:
		bool initialized;

		std::map<std::string, std::string> renderOptions;
	};
}

#endif