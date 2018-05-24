#version 150

in vec3 normal;
in vec3 position;
in vec2 texCoord;

uniform sampler2D tex;
uniform mat4 worldToView;
uniform vec3 starColor[2];
uniform vec3 lightPositions[2];
uniform bool isDirectional[2];
uniform float specularExponent[2];
uniform int offset[1];

out vec4 outColor;

void main(void)
{
	outColor = vec4(0,0,0,0);

	int i = 0;
	int y = 2;

	if(offset[0] == 1)
	{
		y = 1;
	}
	else if(offset[0] == 2) {
		i = 1;
	}
	else if(offset[0] == 3)
		i = 2;

	for(i; i < y; i++) {
		vec3 light_dir = mat3(worldToView)*lightPositions[i];
		vec4 light_pos_view = worldToView*vec4(lightPositions[i],1);
		vec3 light_pos = vec3(light_pos_view) - position;

		vec3 light = light_dir*float(isDirectional[i]) + light_pos*(1.0 - float(isDirectional[i]));

		float shade = dot(normal,normalize(light));
		shade = clamp(shade, 0 ,1);
		vec3 shadeColor = shade*starColor[i];

		outColor += vec4(shadeColor, 1);
		//Specular shading
		vec3 reflectedLight = reflect(-normalize(light), normal);
		vec3 eyeDirection = normalize(-position);
		float specularStrength = 0.0;
		if(dot(light, normal) > 0.0) {
			specularStrength = dot(reflectedLight, eyeDirection);
			specularStrength = max(specularStrength, 0.01);
			specularStrength = pow(specularStrength, specularExponent[i]); 	// 1 is the specular exponent
		}
		outColor += vec4(specularStrength*starColor[i], 0.7);
	}
	outColor = vec4(vec3(outColor),0.7);
	outColor = outColor*texture(tex, texCoord);
}
