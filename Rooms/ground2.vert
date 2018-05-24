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
out vec3 reflectedView;

void main(void)
{
    mat4 mtw = flip * modelToWorld;
	gl_Position = projMatrix * worldToView * mtw * vec4(inPosition, 1.0);
    
	mat3 normalMatrix = transpose(inverse(mat3(worldToView * mtw)));
	normal  = normalize(normalMatrix*inNormal);
	texCoord = inTexCoord;
	position = vec3(worldToView * mtw*vec4(inPosition, 1.0));

	//Enviroment mapping from tutorial
	vec3 viewDirectionInViewCoord = normalize(position);
	vec3 viewDirectionInWorldCoord = inverse(mat3(worldToView)) * viewDirectionInViewCoord;
	vec3 wcNormal = mat3(mtw) * inNormal;
	reflectedView = reflect(viewDirectionInWorldCoord, normalize(wcNormal));
}
