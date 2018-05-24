#version 150

in vec3 inPosition;
//in vec3 inNormal;
in vec2 inTexCoord;

uniform mat4 projMatrix;
uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform float t;
uniform sampler2D vertexMap;
uniform mat4 flip;

out vec3 normal;
out vec2 texCoord;
out vec3 position;
out vec3 posWorld;

void main(void)
{
	vec2 texCoord2;
	texCoord2.s = gl_InstanceID/512.0;
	texCoord2.t = mod(gl_InstanceID,512.0);
	vec4 texel = textureLod(vertexMap, texCoord2/512.0, 0.0);
	vec3 vertex = inPosition;

	mat4 T;
	T[0] = vec4(0.8,0.0,0.0,0.0); //First column
	T[1] = vec4(0.0,6.0,0.0,0.0);
	T[2] = vec4(0.0,0.0,1.0,0.0);
	T[3] = vec4(-400+900*texel.r,100*texel.g, -200 + 900*texel.b, 1.0);

	//Billboard should face viewing plane
	mat4 matrix = worldToView * flip * T;
	matrix[0][0] = 1.0;
	matrix[0][1] = 0.0;
	matrix[0][2] = 0.0;
	matrix[1][0] = 0.0;
	matrix[1][1] = 1.0;
	matrix[1][2] = 0.0;
	matrix[2][0] = 0.0;
	matrix[2][1] = 0.0;
	matrix[2][2] = 1.0;

	texCoord.s = inPosition.x+0.5;
	texCoord.t = inPosition.y+0.5;


	posWorld = vec3(flip * T * vec4(inPosition, 1.0)); //Not sure if this is correct

	//Keep the scaling that was lost
	T[3] = vec4(0.0,0.0,0.0,1.0);

	gl_Position = projMatrix * matrix * T * vec4(inPosition, 1.0);

	mat3 normalMatrix = transpose(inverse(mat3(matrix*T)));
	normal  = normalize(normalMatrix*vec3(0.0, 0.0, 1.0));
	position = vec3(matrix*T*vec4(inPosition, 1.0));
}
