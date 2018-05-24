#version 150

in vec3 inPosition;
in vec2 inTexCoord;

uniform mat4 projMatrix;
uniform mat4 worldToView;

out vec2 texCoord;

void main(void)
{
	texCoord.s = inPosition.x/2.0+0.5;
	texCoord.t = inPosition.y/2.0+0.5;
	gl_Position = vec4(inPosition, 1.0);
}
