#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

uniform mat4 projMatrix;
uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform mat4 flip;

out vec3 normal;
out vec2 texCoord;
out vec3 position;

void main(void)
{
    mat4 mtw = modelToWorld;//flip * modelToWorld;
	gl_Position = projMatrix * worldToView * flip * mtw * vec4(inPosition, 1.0);
	mat3 normalMatrix = transpose(inverse(mat3(worldToView *flip* mtw)));
	normal  = normalize(normalMatrix*inNormal);
	texCoord = inTexCoord;
	position = vec3(worldToView *flip* mtw *vec4(inPosition, 1.0));
}
