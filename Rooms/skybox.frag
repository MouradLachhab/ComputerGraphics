#version 150


out vec4 out_Color;
in vec2 texCoord;
in vec4 position;

uniform sampler2D tex;


void main(void)
{			
	out_Color = texture(tex, texCoord);
}
