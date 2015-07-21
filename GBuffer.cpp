
#include "GBuffer.h"

using namespace engine;
using namespace sgl;

GBuffer::GBuffer() : 
	_normalTexture(Texture::Target::TEXTURE2D),
	_diffuseTexture(Texture::Target::TEXTURE2D),
	_colorTexture(Texture::Target::TEXTURE2D)
{
}

void GBuffer::bindForWriting()
{
	_fbo.bind();
}

void GBuffer::bindForReading()
{
	unbind();

	_normalTexture.bind(Texture::Unit::T0);
	_diffuseTexture.bind(Texture::Unit::T1);
	_colorTexture.bind(Texture::Unit::T2);
}

void GBuffer::unbind()
{
	_fbo.unbind();
}

void GBuffer::init(int width, int height)
{
	initTexture(_normalTexture, width, height);
	initTexture(_diffuseTexture, width, height);
	initTexture(_colorTexture, width, height);

	// init the depth buffer
	_depthBuffer.bind();
	_depthBuffer.storage(RenderBuffer::Storage::DEPTH, width, height);
	_depthBuffer.unbind();

	_fbo.bind();

	// set the depths buffer
	_fbo.setRenderBuffer(_depthBuffer, FrameBuffer::Attachment::DEPTH);

	// add multiple render targets
	_fbo.addMRT(_normalTexture);
	_fbo.addMRT(_diffuseTexture);
	_fbo.addMRT(_colorTexture);
	_fbo.setMRTBuffers();

	// check
	_fbo.checkError();

	_fbo.unbind();
}

Texture& GBuffer::getNormalTexture()
{
	return _normalTexture;
}

Texture& GBuffer::getDiffuseTexture()
{
	return _diffuseTexture;
}

Texture& GBuffer::getColorTexture()
{
	return _colorTexture;
}

void GBuffer::initTexture(sgl::Texture& texture, int width, int height)
{
	texture.bind();

	texture.setPixelDataType(GLType::FLOAT);

	texture.setWidth(width);
	texture.setHeight(height);

	texture.setFormat(Texture::Format::RGB);
	texture.setInternalFormat(Texture::InternalFormat::RGB);

	texture.setData(0);

	texture.parameter(Texture::ParamName::MAG_FILTER, Texture::Param::LINEAR);
	texture.parameter(Texture::ParamName::MIN_FILTER, Texture::Param::LINEAR);

	texture.unbind();
}

GBuffer::~GBuffer()
{
}

