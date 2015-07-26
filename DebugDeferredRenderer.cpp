
#include "DebugDeferredRenderer.h"

#include "VoxelEngine.h"
#include "GLSL_GeometryPass.h"
#include "GLSL_LightPass.h"

#include <SGL/Util/Context.h>

using namespace engine;
using namespace sgl;

DebugDeferredRenderer::DebugDeferredRenderer(void)
{
}

void DebugDeferredRenderer::init()
{
	try
	{
		// load geometry pass shader
		_geometryPass.load(ShaderProgram::Type::VERTEX,   GLSL_GEOMETRYPASS_VERT);
		_geometryPass.load(ShaderProgram::Type::FRAGMENT, GLSL_GEOMETRYPASS_FRAG);

		_geometryPass.addAttribute("vPosition");
		_geometryPass.addAttribute("vNormal");
		_geometryPass.addAttribute("vTexCoord");

		_geometryPass.bindFragOutput("outNormal");
		_geometryPass.bindFragOutput("outDiffuse");
		_geometryPass.bindFragOutput("outColor");

		_geometryPass.link();

		// load the light pass shader
		_lightPass.load(ShaderProgram::Type::VERTEX,   GLSL_LIGHTPASS_VERT);
		_lightPass.load(ShaderProgram::Type::FRAGMENT, GLSL_DEBUG_LIGHTPASS_FRAG);

		_lightPass.addAttribute("vPosition");

		_lightPass.link();

		// initialize the GBuffer
		_gBuffer.init((int)Context::getViewPortWidth(), (int)Context::getViewPortHeight());

		// init the screen mesh
		initScreenMesh(_screenMesh);

	}
	catch (Exception& e)
	{
		std::cout << "Error initializing the Renderer" << std::endl;

		throw std::exception(e.what());
	}

	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);

	initialized = true;

	setRenderOption("sampler", "diffuse-map");
}

void DebugDeferredRenderer::begin()
{
	// clear the back buffer
	Context::clear(Context::BufferBits::COLOR_DEPTH);

	// bind to write to the gbuffer
	_gBuffer.bindForWriting();

	// clear the gbuffer 
	Context::clear(Context::BufferBits::COLOR_DEPTH);

	// start the geometry pass
	_geometryPass.begin();

	// depth testing is required
	glEnable(GL_DEPTH_TEST);
}

void DebugDeferredRenderer::render(ChunkManager& chunkManager, sgl::Matrix4& VP)
{
	// get the chunkmanager transform matrix and calculate the MVP

	Matrix4& M = chunkManager.getWorldTransform();
	Matrix4 MVP = VP * M;
	Matrix3 N = M.toNormalMatrix();

	_geometryPass["MVP"].set(MVP);
	_geometryPass["N"].set(N);

	Texture& texture = VoxelEngine::getEngine()->getResources().getTextureManager().getTexture(chunkManager.getAtlasName());
	texture.bind(Texture::Unit::T0);

	_geometryPass["blockTexture"].set(texture);

	chunkManager.render();

	texture.unbind();
}

void DebugDeferredRenderer::end()
{
	_geometryPass.end();

	// render full screen quad using the light pass shader

	// bind gbuffer to access textures
	//_gBuffer.bindForReading();
	_gBuffer.unbind();
	Texture& texture = _gBuffer.getTexture(renderOptions["sampler"]);

	texture.bind(Texture::Unit::T0);

	// disable depth testing as it's not needed in thsi pass
	glDisable(GL_DEPTH_TEST);

	_lightPass.begin();
	{
		_lightPass["screenSize"].set(Context::getViewPortDimensions());

		_lightPass["sampler"].set(texture);

		_screenMesh.bind();
		_screenMesh.draw();
		_screenMesh.unbind();
	}
	_lightPass.end();
}

void DebugDeferredRenderer::initScreenMesh(sgl::Mesh& mesh)
{
	// float buffer with screen space coordinates of quad
	float screenMeshBuffer[] = {
		-1,  1,
		 1,  1,
		-1, -1,

		 1,  1,
		-1, -1,
		 1, -1
	};

	mesh.addAttribute(VertexAttribute(0, 2));
	mesh.create(2 * sizeof(float));
	mesh.setDrawCount(6);

	Buffer& vbo = mesh.getVBO();
	vbo.bind();
	vbo.setData(screenMeshBuffer, sizeof(screenMeshBuffer));
	vbo.unbind();
}

DebugDeferredRenderer::~DebugDeferredRenderer()
{
}