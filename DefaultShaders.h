
#ifndef DEFAULTSHADERS_H
#define DEFAULTSHADERS_H

#include <string>

#define GLSL(src) "\n" #src

//
static std::string GLSL_VERTEX_NORMAL_TEXTURE_VERT = GLSL(

	in vec3 vPosition;
	in vec3 vNormal;
	in vec2 vTexCoord;

	out vec3 fNormal;
	out vec2 fTexCoord;
	
	uniform mat4 M;
	uniform mat4 MVP;

	void main()
	{
		gl_Position = MVP * vec4(vPosition, 1);

		fNormal   = vNormal;
		fTexCoord = vTexCoord;
	}
);

static std::string GLSL_VERTEX_NORMAL_TEXTURE_FRAG = GLSL(

	out vec4 fragColor;

	in vec3 fNormal;
	in vec2 fTexCoord;

	uniform sampler2D blockTexture;

	void main()
	{
		fragColor = texture(blockTexture, fTexCoord);
	}

);

#endif
