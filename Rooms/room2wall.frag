#version 150

out vec4 out_Color;
in vec2 texCoord;
in vec3 normal;
in vec3 position;

uniform vec3 lightSourcesDirPosArr[5];
uniform vec3 lightSourcesColorArr[5];
uniform float specularExponent;
uniform float Kspecular;
uniform float Kdiffuse;
uniform float Kambient;
uniform bool isDirectional[5];
uniform float spotlightCos[5];
uniform vec3 spotlightDirectionArr[5];
uniform sampler2D tex;
uniform mat4 worldToView;
uniform mat4 flip;

void main(void)
{
	out_Color = vec4(0,0,0,0);
	float shade;
    float attenuation = 1.0;
	
	//Ambient light
	vec4 ambientLight = vec4(0.4, 0.4, 0.4, 1.0);

	int i = 0;
	for(i; i < 5; i++) {	

		vec3 light_dir_vector = normalize(mat3(worldToView)*lightSourcesDirPosArr[i]);
		vec4 light_pos_view = worldToView*flip*vec4(lightSourcesDirPosArr[i],1);
		vec3 light_pos_vector = vec3(light_pos_view) - position;

		light_pos_vector = normalize(light_pos_vector);

        //Spotlight
        vec3 spotlightDirection = normalize(mat3(worldToView)*spotlightDirectionArr[i]);
		float cosTheta = dot(spotlightDirection,-light_pos_vector);
        if( cosTheta > 0.5)
        {
            attenuation = pow(cosTheta,2);
        }
		else {
			attenuation = 0.0;
		}

		vec3 light = light_dir_vector*float(isDirectional[i]) + light_pos_vector*(1.0 - float(isDirectional[i]));
		shade = dot(normalize(normal), light);
		shade = clamp(shade, 0 , 1);
		vec3 diffuse = Kdiffuse*shade*lightSourcesColorArr[i];

		//Specular shading
		vec3 reflectedLight = reflect(-light, normalize(normal));
		vec3 eyeDirection = normalize(-position);
		float specularStrength = 0.0;
		if(dot(light, normal) > 0.0) {
			specularStrength = dot(reflectedLight, eyeDirection);
			specularStrength = max(specularStrength, 0.01);
			specularStrength = pow(specularStrength, specularExponent);
			}
		vec3 specular = Kspecular*specularStrength*lightSourcesColorArr[i];
		out_Color += attenuation*vec4(diffuse+specular, 1);
	}
	out_Color = (Kambient*ambientLight + out_Color)*texture(tex,texCoord);
}
