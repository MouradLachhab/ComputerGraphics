#version 150

uniform mat4 projectionMatrix;
uniform mat4 camMatrix;

in vec3 inPosition;
in vec2 inTexCoord;
out vec2 texCoord;
out vec4 position;

void main(void)
{
	position = projectionMatrix*camMatrix*vec4(inPosition, 1.0);
	gl_Position = position;
	texCoord = inTexCoord;
}
