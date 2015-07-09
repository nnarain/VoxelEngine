
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

		out vec3 fNormal;
		out vec2 fTexCoord;

		uniform mat4 MVP;
		uniform mat4 M;
		uniform mat3 N;

		void main()
		{
			gl_Position = MVP * vec4(vPosition, 1);

			fNormal = N * vNormal;
			fTexCoord = vTexCoord;
		}
	);

	const std::string GLSL_GEOMETRYPASS_FRAG = GLSL(

		out vec3 outNormal;
		out vec3 outDiffuse;

		in vec3 fNormal;
		in vec2 fTexCoord;

		uniform sampler2D sampler;

		void main()
		{
			outNormal = normalize(fNormal);
			outDiffuse = texture(sampler, fTexCoord).xyz;
		}
	);
}

#endif
