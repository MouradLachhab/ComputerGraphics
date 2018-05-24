// Select texture set by setting this constant to 0 or 6
#define TEXTURE_OFFSET 0

// Skybox
GLuint cubemap, cubemap1, cubemap2, cubemap3;
Model *box[6];

GLfloat vertices[6][6*3] =
{
	{ // +x
		0.5,-0.5,-0.5,		// 1
		0.5,0.5,-0.5,		// 2
		0.5,0.5,0.5,			// 6
		0.5,-0.5,0.5,		// 5
	},
	{ // -x
		-0.5,-0.5,-0.5,		// 0 -0
		-0.5,-0.5,0.5,		// 4 -1
		-0.5,0.5,0.5,		// 7 -2
		-0.5,0.5,-0.5,		// 3 -3
	},
	{ // +y
		0.5,0.5,-0.5,		// 2 -0
		-0.5,0.5,-0.5,		// 3 -1
		-0.5,0.5,0.5,		// 7 -2
		0.5,0.5,0.5,			// 6 -3
	},
	{ // -y
		-0.5,-0.5,-0.5,		// 0
		0.5,-0.5,-0.5,		// 1
		0.5,-0.5,0.5,		// 5
		-0.5,-0.5,0.5		// 4
	},
	{ // +z
		-0.5,-0.5,0.5,		// 4
		0.5,-0.5,0.5,		// 5
		0.5,0.5,0.5,			// 6
		-0.5,0.5,0.5,		// 7
	},
	{ // -z
		-0.5,-0.5,-0.5,	// 0
		-0.5,0.5,-0.5,		// 3
		0.5,0.5,-0.5,		// 2
		0.5,-0.5,-0.5,		// 1
	}
};

GLfloat texcoord[6][6*2] =
{
	{
		1.0, 1.0,
		1.0, 0.0, // left OK
		0.0, 0.0,
		0.0, 1.0,
	},
	{
		0.0, 1.0, // right OK
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0,
	},
	{
		1.0, 0.0, // top OK
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
	},
	{
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0, // bottom
		0.0, 0.0,
	},
	{
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0, // back OK
		0.0, 0.0,
	},
	{
		1.0, 1.0,
		1.0, 0.0, // front OK
		0.0, 0.0,
		0.0, 1.0,
	}
};
GLuint indices[6][6] =
{
	{0, 2, 1, 0, 3, 2},
	{0, 2, 1, 0, 3, 2},
	{0, 2, 1, 0, 3, 2},
	{0, 2, 1, 0, 3, 2},
	{0, 2, 1, 0, 3, 2},
	{0, 2, 1, 0, 3, 2}
};

char *textureFileName[24] =
{
	// Buffer Room
	"textures/whiteback.tga",
	"textures/whitefront.tga",
	"textures/whitetop.tga",
	"textures/whitebottom.tga",
	"textures/whiteright.tga",
	"textures/whiteleft.tga",

	// Room1
	"textures/back.tga",
	"textures/front.tga",
	"textures/top.tga",
	"textures/bottom.tga",
	"textures/right.tga",
	"textures/left.tga",

	// Room2
	"./textures/DarkStormy/DarkStormyLeft2048.tga",
	"./textures/DarkStormy/DarkStormyRight2048.tga",
	"./textures/DarkStormy/DarkStormyUp2048.tga",
	"./textures/DarkStormy/DarkStormyDown2048.tga",
	"./textures/DarkStormy/DarkStormyFront2048.tga",
	"./textures/DarkStormy/DarkStormyBack2048.tga",

	// Room3
	"textures/sb_graveyard/grave_ft.tga",
	"textures/sb_graveyard/grave_bk.tga",
	"textures/sb_graveyard/grave_up.tga",
	"textures/sb_graveyard/grave_dn.tga",
	"textures/sb_graveyard/grave_rt.tga",
	"textures/sb_graveyard/grave_lf.tga"

};

TextureData skyboxes[4][12];

void loadTextures(int z)
{
	int i;
	int j = 6 + z*6;
	int k = 0;
	glActiveTexture(GL_TEXTURE0); // Just make sure the texture unit match
	// Load texture data and create ordinary texture objects (for skybox)
	for (i = z * 6; i < j; i++)
	{
		printf("Loading texture %s in postion %d\n", textureFileName[i], z);
		LoadTGATexture(textureFileName[i], &skyboxes[z][k++]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

}
