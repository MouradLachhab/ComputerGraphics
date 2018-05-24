#version 150

out vec4 out_Color;
in vec2 texCoord;
uniform float t;
uniform sampler2D posTex; //position
uniform sampler2D speedTex; //speed

/*
Store pos x, y, z, size in one texture, that gets updated.
posTex.r = x
posTex.g = y
posTex.b = z
posTex.a = size
Store speed dx,dy,dz in one texture, use to update position.
speedTex.r = dx
speedTex.g = dy
speedTex.b = dz
*/

void main(void)
{
	vec4 pos = texture(posTex,texCoord);
	vec4 speed = texture(speedTex,texCoord);
	float dx = 0.002*(0.5-speed.r);
	float dy = -0.01*(0.8+(1-0.8)*speed.g);
	float dz = 0.002*(0.5-speed.b);
	float x = pos.r;// + dx;
	float y = pos.g + dy;
	float z = pos.b;// + dz;
	float size = pos.a;
	if(y<0.001*speed.r)
	{
		y=speed.b;//0.3+(1-0.3)*speed.b;
		//y=1.0;
	};
		
	out_Color = vec4(x, y, z, size);
}
