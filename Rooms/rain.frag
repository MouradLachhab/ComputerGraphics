#version 150

out vec4 out_Color;
in vec2 texCoord;
in vec3 normal;
in vec3 position;
in vec3 posWorld;

uniform vec3 lightSourcesDirPosArr[5];
uniform vec3 lightSourcesColorArr[5];
uniform float specularExponent;
uniform float Kspecular;
uniform float Kdiffuse;
uniform float Kambient;
uniform bool isDirectional[5];
uniform float spotlightCos[5];
uniform vec3 spotlightDirectionArr[5];
uniform sampler2D concTex, tex, map;
uniform mat4 worldToView;
uniform mat4 flip;
uniform bool flipped[4]; 

void main(void)
{
	out_Color = vec4(0,0,0,0);
	//float shade;
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
			//Testing to see if better without 
            attenuation = 1.0;//pow(cosTheta,7);
			out_Color = out_Color + vec4(lightSourcesColorArr[i], 1.0);
        }
		else {
			attenuation = attenuation + 0.0;
		}
	}
	vec4 t = (Kambient*ambientLight + attenuation*out_Color)*texture(tex,texCoord);
	vec4 t2 = texture(tex,texCoord);
	if(flipped[2]) {
		if(posWorld.x > -2)
			discard;
	}
	else {
		if(posWorld.x < 2)
			discard;
	}
	out_Color = vec4(vec3(t.r, t.g, t.b), 0.5*t2.a);
}
