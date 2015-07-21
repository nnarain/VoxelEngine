
#ifndef GLSL_GEOMETRYPASS_H
#define GLSL_GEOMETRYPASS_H

#define GLSL(src) "\n" #src

#include <string>

namespace sgl
{
	const std::string GLSL_GEOMETRYPASS_VERT = GLSL(
		in vec3 vPosition;
		in vec3 vNormal;
		in vec2 vTexCoord;
		in vec3 vColor;

		out vec3 fNormal;
		out vec2 fTexCoord;
		out vec3 fColor;

		uniform mat4 MVP;
		uniform mat3 N;

		void main()
		{
			gl_Position = MVP * vec4(vPosition, 1);

			fNormal   = N * vNormal;
			fTexCoord = vTexCoord;
			fColor    = vColor;
		}
	);

	const std::string GLSL_GEOMETRYPASS_FRAG = GLSL(

		out vec3 outNormal;
		out vec3 outDiffuse;
		out vec3 outColor;

		in vec3 fNormal;
		in vec2 fTexCoord;
		in vec3 fColor;

		uniform sampler2D blockTexture;

		void main()
		{
			outNormal  = normalize(fNormal);
			outDiffuse = texture(blockTexture, fTexCoord).xyz;
			outColor   = fColor;
		}
	);
}

#endif
