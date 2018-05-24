#version 150

out vec4 out_Color;
in vec2 texCoord;
in vec3 normal;
in vec3 position;
in vec3 reflectedView;

uniform vec3 lightSourcesDirPosArr[5];
uniform vec3 lightSourcesColorArr[5];
uniform float specularExponent;
uniform float Kspecular;
uniform float Kdiffuse;
uniform bool isDirectional[5];
uniform float spotlightCos;
uniform vec3 spotlightDirectionArr[5];
uniform sampler2D concTex, grassTex, map;
uniform mat4 worldToView;
uniform samplerCube cubemap;
uniform mat4 flip;

void main(void)
{
	out_Color = vec4(0,0,0,0);
	float shade;
    vec4 m = normalize(texture(map, vec2(16,16)*texCoord));
    float attenuation = 1.0;
	
	//Ambient light
	vec4 ambientLight = vec4(0.4, 0.4, 0.4, 1.0);

	//Fake bump mapping from tutorial
	const float step = 1.0/512.0;
	float bt = texture(map, vec2(30,30)*texCoord).r - texture(map, vec2(30,30)*texCoord + vec2(0.0, step)).r;
	float bs = texture(map, vec2(30,30)*texCoord).r - texture(map, vec2(30,30)*texCoord + vec2(step, 0.0)).r;
	vec3 ps, pt;
	ps = 2.0 * cross(normal, vec3(1.0, 0.0, 0.0));
	pt = 2.0 * cross(normal, ps);
	vec3 n = normal + ps * bs + pt * bt;
	//n = normal;

	int i = 0;
	for(i; i < 5; i++) {	

		vec3 light_dir_vector = normalize(mat3(worldToView)*lightSourcesDirPosArr[i]);
		vec4 light_pos_view = worldToView*flip*vec4(lightSourcesDirPosArr[i],1);
		vec3 light_pos_vector = vec3(light_pos_view) - position;

		light_pos_vector = normalize(light_pos_vector);

        //Spotlight
		if(!isDirectional[i])
		{
			vec3 spotlightDirection = normalize(mat3(worldToView)*spotlightDirectionArr[i]);
			float cosTheta = dot(spotlightDirection,-light_pos_vector);
			if( cosTheta > spotlightCos)
			{
				attenuation = pow(cosTheta,7);
			}
			else {
				attenuation = 0.0;
			}
		}

		vec3 light = light_dir_vector*float(isDirectional[i]) + light_pos_vector*(1.0 - float(isDirectional[i]));
        //vec3 light = light_pos;
		shade = dot(normalize(n), light);
		shade = clamp(shade, 0 , 1);
		vec3 diffuse = Kdiffuse*shade*lightSourcesColorArr[i];

		//Specular shading
		vec3 reflectedLight = reflect(-light, normalize(n));
		vec3 eyeDirection = normalize(-position);
		float specularStrength = 0.0;
		if(dot(light, n) > 0.0) {
			specularStrength = dot(reflectedLight, eyeDirection);
			specularStrength = max(specularStrength, 0.01);
			specularStrength = pow(specularStrength, specularExponent);
			}
		vec3 specular = Kspecular*specularStrength*lightSourcesColorArr[i];
		out_Color += attenuation*vec4(diffuse+specular, 1);
	}

	out_Color = (Kdiffuse*ambientLight + out_Color) * texture(grassTex, vec2(20,16)*texCoord);

	//Environment mapping to get wet effect
	out_Color = (1.0-m)*out_Color+m*texture(cubemap, normalize(reflectedView));
}
