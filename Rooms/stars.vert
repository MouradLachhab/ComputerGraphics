#version 150

in vec3 inPosition;

uniform mat4 projMatrix;
uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform mat4 flip;

void main(void)
{
	gl_Position = projMatrix * worldToView * flip *  modelToWorld * vec4(inPosition, 1.0);
}
