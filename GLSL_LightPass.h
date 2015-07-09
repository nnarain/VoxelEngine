


#ifndef GLSL_LIGHTPASS_H
#define GLSL_LIGHTPASS_H

#define GLSL(src) "\n" #src

#include <string>

namespace sgl
{
	const std::string GLSL_LIGHTPASS_VERT = GLSL(
		in vec2 vPosition;

		void main()
		{
			gl_Position = vec4(vPosition, 0, 1);
		}
	);

	const std::string GLSL_LIGHTPASS_FRAG = GLSL(

		out vec3 fragColor;

		uniform vec2 screenSize;

		uniform sampler2D normalMap;
		uniform sampler2D diffuseMap;

		vec2 calcTexCoord();

		void main()
		{
			vec2 texCoord = calcTexCoord();

			vec3 normal  = texture(normalMap, texCoord).xyz;
			vec3 diffuse = texture(diffuseMap, texCoord).xyz;

			vec3 diffuseColor = diffuse;

			float diffuseFactor = dot(normal, vec3(1, 1, 1));

			if (diffuseFactor > 0)
			{
				diffuseColor *= vec3(1, 1, 1) * vec3(1, 1, 1) * 1 * diffuseFactor;
			}

			fragColor = diffuseColor;
		}

		vec2 calcTexCoord()
		{
			return gl_FragCoord.xy / screenSize;
		}

	);
}

#endif
