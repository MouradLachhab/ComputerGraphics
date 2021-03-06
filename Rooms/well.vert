#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

uniform mat4 projMatrix;
uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform mat4 flip;

out vec3 normal;
out vec3 position;
out vec2 texCoord;

void main(void)
{
	texCoord = inTexCoord;
	gl_Position = projMatrix * worldToView * flip * modelToWorld * vec4(inPosition, 1.0);

	mat3 normalMatrix = mat3(worldToView * modelToWorld);

	normal  = normalize(normalMatrix*inNormal);
	position = vec3(worldToView * modelToWorld*vec4(inPosition, 1.0));

}
