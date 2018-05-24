// Rainy room
// Room with effects like rain, splashing on floor, wet floor, street lamps with spotlights

#include "room2.h"
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

//Extern
extern GLint flipped[];

GLuint lampProgram, ground2Program, rainProgram, texProgram, splashProgram;
int renderTo;

//Objects
Model *ground, *lamp[3], *quad;

//Vertex array object
unsigned int vertexArrayObjID, vertexArrayObjID2;

//Textures
GLuint groundTex, grassTex, mapTex, raindrop, posTex1, posTex2, speedTex, splash;
GLuint cubeTex;
TextureData lampTex[3];

//Time
GLint t = 0;
GLfloat tSplash = 0.0;

GLuint fb1, fb2;

//Cupemap for environment mapping
void loadcubemap() {
    
    TextureData texData[6];

	// Texture skybox 1
	glGenTextures(1, &cubeTex);
	glActiveTexture(GL_TEXTURE0);

    printError("Generate textureids");

	LoadTGATexture("./textures/DarkStormy/DarkStormyLeft2048.tga", &texData[0]);
	LoadTGATexture("./textures/DarkStormy/DarkStormyRight2048.tga", &texData[1]);
	LoadTGATexture("./textures/DarkStormy/DarkStormyUp2048.tga", &texData[2]);
	LoadTGATexture("./textures/DarkStormy/DarkStormyDown2048.tga", &texData[3]);
	LoadTGATexture("./textures/DarkStormy/DarkStormyBack2048.tga", &texData[4]);
	LoadTGATexture("./textures/DarkStormy/DarkStormyFront2048.tga", &texData[5]);

    printError("Load TGA Texture");

	//Load to cubemap
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, texData[0].w, texData[0].h, 0, GL_RGB, GL_UNSIGNED_BYTE, texData[0].imageData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, texData[1].w, texData[1].h, 0, GL_RGB, GL_UNSIGNED_BYTE, texData[1].imageData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, texData[2].w, texData[2].h, 0, GL_RGB, GL_UNSIGNED_BYTE, texData[2].imageData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, texData[3].w, texData[3].h, 0, GL_RGB, GL_UNSIGNED_BYTE, texData[3].imageData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, texData[4].w, texData[4].h, 0, GL_RGB, GL_UNSIGNED_BYTE, texData[5].imageData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, texData[5].w, texData[5].h, 0, GL_RGB, GL_UNSIGNED_BYTE, texData[4].imageData);

    printError("Load to cubemap");

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// MIPMAPPING
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	
	printError("Load Textures");

}

//Initializes the framebuffers etc that is needed
//to render to texture for updating the rain positions
void initRain(mat4 projectionMatrix)
{
	glDisable(GL_DEPTH_TEST);
	//GLuint currentfbo;
	//glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentfbo);

	GLfloat quadVertices[] = {
		-1.0f,-1.0f,0.0f,
		-1.0f,1.0f,0.0f,
		1.0f,-1.0f,0.0f,
		1.0f,1.0f,0.0f };

	GLuint indices[] =
	{ 0, 1, 2, 1, 2, 3 };

	quad = LoadDataToModel(
		quadVertices,
		NULL,
		NULL,
		NULL,
		indices,
		4,
		6);


	renderTo = 2;
	
	texProgram = loadShaders("raintex.vert", "raintex.frag");
	glUseProgram(texProgram);

	glUniform1f(glGetUniformLocation(texProgram, "t"), t);
	glUniform1i(glGetUniformLocation(texProgram, "posTex"), 1);
	glUniform1i(glGetUniformLocation(texProgram, "speedTex"), 2);
	LoadTGATextureSimple("./textures/noise.tga", &speedTex);

	mat4 projm = ortho(0.0, 512.0, 0.0, 512.0, -1.0, 1.0);
	glUniformMatrix4fv(glGetUniformLocation(texProgram, "projMatrix"), 1, GL_TRUE, projm.m);

	//posTex1
	LoadTGATextureSimple("./textures/noise.tga", &posTex1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//posTex2
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &posTex2);
	glBindTexture(GL_TEXTURE_2D, posTex2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, NULL);

	//Framebuffer 1
	glGenFramebuffers(1, &fb1);
	glBindFramebuffer(GL_FRAMEBUFFER, fb1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex1, 0);
	//CHECK_FRAMEBUFFER_STATUS();

	//Framebuffer 2
	glGenFramebuffers(1, &fb2);
	glBindFramebuffer(GL_FRAMEBUFFER, fb2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex2, 0);
	//CHECK_FRAMEBUFFER_STATUS();
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	printError("Init texRain");
}

void room2Init(mat4 projectionMatrix)
{
    loadcubemap();
    mat4 modelToWorld = IdentityMatrix();
	//Load flat ground
	GLfloat vertices[] =
		{0.0f,-100.0f,-100.0f,
		100.0f,-100.0f,-100.0f,
		100.0f,-100.0f,100.0f,
		0.0f,-100.0f,100.0f};

	GLfloat normals[] =
	{
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	
	GLfloat texcoord[] =
	{
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
	};
	GLuint indices[] =
	{0, 2, 1, 0, 3, 2};
	
	ground = LoadDataToModel(
		vertices,
		normals,
		texcoord,
		NULL,
		indices,
		4,
		6);

	ScaleModel(ground, 4, 1, 0.5);
	ReloadModelData(ground);

	printError("load ground");

	// Load objects
	lamp[0] = LoadModelPlus("./objects/streetlamp/Models/base.obj");
	lamp[1] = LoadModelPlus("./objects/streetlamp/Models/top.obj");
	lamp[2] = LoadModelPlus("./objects/streetlamp/Models/light.obj");

	printError("Load lamps");

	// Load and compile shader
	lampProgram = loadShaders("room2.vert", "room2.frag");
	glUseProgram(lampProgram);
	printError("init shader lamp");
	glUniformMatrix4fv(glGetUniformLocation(lampProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(lampProgram, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);

    // Light
    Point3D lightSourcesColorsArr[] = { {1.0f, 0.8f, 0.5f}, // Yellow light for street lamps

                                 { 1.0f, 0.8f, 0.5f },

                                 { 1.0f, 0.8f, 0.5f },

								 { 1.0f, 0.8f, 0.5f },

                                 { 1.0f, 0.8f, 0.5f } };



    GLfloat specularExponent[] = {10.0, 20.0, 60.0, 5.0};

    GLint isDirectional[] = {0,0,0,0,0};


    Point3D lightSourcesDirectionsPositions[] = { {8.0f, 9.0f, 10.0f}, // Spotlight for lamp. Maybe move a tiny bit more

                                           { 40+8.0f, 9.0f, 10.0f },

                                           { 2*40+8.0f, 9.0f, 10.0f },

                                           { 3*40+8.0f, 9.0f, 10.0f },
											
										   { 4*40+8.0f, 9.0f, 10.0f } }; //
    //Spotlight
	GLfloat spotlightCos[] = { 0.5, 0.99, 0.99, 0.99 };

	Point3D spotlightDirectionArr[] = { { 0.0f, -1.0f, 0.0f }, // Spotlight

	{ 0.0f, -1.0f, 0.0f }, // Directional light. Moon

	{ 0.0f, -1.0f, 0.0f },
		
	{ 0.0f, -1.0f, 0.0f }, // 

	{ 0.0f, -1.0f, 0.0f } }; //

    glUniform3fv(glGetUniformLocation(lampProgram, "lightSourcesDirPosArr"), 5, &lightSourcesDirectionsPositions[0].x);

	glUniform3fv(glGetUniformLocation(lampProgram, "lightSourcesColorArr"), 5, &lightSourcesColorsArr[0].x);

	glUniform1f(glGetUniformLocation(lampProgram, "specularExponent"), specularExponent[0]);

	glUniform1iv(glGetUniformLocation(lampProgram, "isDirectional"), 5, isDirectional);

	glUniform3fv(glGetUniformLocation(lampProgram, "spotlightDirectionArr"), 5, &spotlightDirectionArr[0].x);

	//glUniform1iv(glGetUniformLocation(lampProgram, "spotlightCos"), 5, spotlightCos);

    // Textures
    LoadTGATextureSimple("./textures/brick_pavement_0022_03_tiled.tga", &groundTex);
	LoadTGATextureSimple("./textures/brick_pavement_0048_01_tiled.tga", &grassTex);
	//LoadTGATextureSimple("./textures/grass90.tga", &grassTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	LoadTGATextureSimple("./textures/watertexture2.tga", &mapTex);

	LoadTGATexture("./objects/streetlamp/Textures/metal2.tga", &lampTex[0]);

	LoadTGATexture("./objects/streetlamp/Textures/metal1.tga", &lampTex[1]);

	LoadTGATexture("./objects/streetlamp/Textures/fiberglass.tga", &lampTex[2]);
	printError("Load lamp textures");

    glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, groundTex);

    glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, grassTex);

    glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mapTex);

    glUniform1i(glGetUniformLocation(lampProgram, "concTex"), 1);
	glUniform1i(glGetUniformLocation(lampProgram, "grassTex"), 2);
    glUniform1i(glGetUniformLocation(lampProgram, "map"), 3);

    printError("Init room2");    

	//Ground program

	ground2Program = loadShaders("ground2.vert", "ground2.frag");
	glUseProgram(ground2Program);

	glUniform3fv(glGetUniformLocation(ground2Program, "lightSourcesDirPosArr"), 5, &lightSourcesDirectionsPositions[0].x);

	glUniform3fv(glGetUniformLocation(ground2Program, "lightSourcesColorArr"), 5, &lightSourcesColorsArr[0].x);

	glUniform1f(glGetUniformLocation(ground2Program, "specularExponent"), specularExponent[0]);

	glUniform1iv(glGetUniformLocation(ground2Program, "isDirectional"), 5, isDirectional);

	glUniform3fv(glGetUniformLocation(ground2Program, "spotlightDirectionArr"), 5, &spotlightDirectionArr[0].x);

	glUniform1f(glGetUniformLocation(ground2Program, "spotlightCos"), spotlightCos[0]);

	glUniform1i(glGetUniformLocation(ground2Program, "cubemap"), 0);
	glUniform1i(glGetUniformLocation(ground2Program, "concTex"), 1);
	glUniform1i(glGetUniformLocation(ground2Program, "grassTex"), 2);
	glUniform1i(glGetUniformLocation(ground2Program, "map"), 3);
	glUniformMatrix4fv(glGetUniformLocation(ground2Program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(ground2Program, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);

	printError("Init ground");

    //room2program for wall
	room2program = loadShaders("room2wall.vert", "room2wall.frag");
	glUseProgram(room2program);

	glUniform3fv(glGetUniformLocation(room2program, "lightSourcesDirPosArr"), 5, &lightSourcesDirectionsPositions[0].x);

	glUniform3fv(glGetUniformLocation(room2program, "lightSourcesColorArr"), 5, &lightSourcesColorsArr[0].x);

	GLfloat specExp = 5;
	GLfloat Kambient = 1.0;
	GLfloat Kspecular = 0.1;
	GLfloat Kdiffuse = 1.0;

	glUniform1f(glGetUniformLocation(room2program, "specularExponent"), specExp);
    glUniform1f(glGetUniformLocation(room2program, "Kspecular"), Kspecular);
    glUniform1f(glGetUniformLocation(room2program, "Kdiffuse"), Kdiffuse);
    glUniform1f(glGetUniformLocation(room2program, "Kambient"), Kambient);

	glUniform1iv(glGetUniformLocation(room2program, "isDirectional"), 5, isDirectional);

	glUniform3fv(glGetUniformLocation(room2program, "spotlightDirectionArr"), 5, &spotlightDirectionArr[0].x);

	glUniform1f(glGetUniformLocation(room2program, "spotlightCos"), spotlightCos[0]);

	glUniform1i(glGetUniformLocation(room2program, "tex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(room2program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(room2program, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);

	// Rain program
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLfloat verticesRain[] = {
		-0.5f,-0.5f,0.0f,
		-0.5f,0.5f,0.0f,
		0.5f,-0.5f,0.0f };
	rainProgram = loadShaders("rain.vert", "rain.frag");
	glUseProgram(rainProgram);

	unsigned int vertexBufferObjID;
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);
	glGenBuffers(1, &vertexBufferObjID);

	//VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), verticesRain, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(rainProgram, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(rainProgram, "inPosition"));

	LoadTGATextureSimple("./textures/rain2.tga", &raindrop);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(rainProgram, "tex"), 0);
	glUniform1i(glGetUniformLocation(rainProgram, "vertexMap"), 1);

	glUniformMatrix4fv(glGetUniformLocation(rainProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	glUniform3fv(glGetUniformLocation(rainProgram, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(rainProgram, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1f(glGetUniformLocation(rainProgram, "specularExponent"), specularExponent[0]);
	glUniform1iv(glGetUniformLocation(rainProgram, "isDirectional"), 4, isDirectional);
	glUniform3fv(glGetUniformLocation(rainProgram, "spotlightDirectionArr"), 4, &spotlightDirectionArr[0].x);
	glUniform1f(glGetUniformLocation(rainProgram, "spotlightCos"), spotlightCos[0]);
	//GLfloat specExp = 5;
	//GLfloat Kambient = 2.0;
	//GLfloat Kspecular = 0.1;
	//GLfloat Kdiffuse = 1.0;
	glUniform1f(glGetUniformLocation(rainProgram, "specularExponent"), specExp);
	glUniform1f(glGetUniformLocation(rainProgram, "Kspecular"), Kspecular);
	glUniform1f(glGetUniformLocation(rainProgram, "Kdiffuse"), Kdiffuse);
	glUniform1f(glGetUniformLocation(rainProgram, "Kambient"), Kambient);

	printError("Init rain");

	// Splash program
	// The billboards are upside down right now...

	splashProgram = loadShaders("rainsplash.vert", "rainsplash.frag");
	glUseProgram(splashProgram);

	unsigned int vertexBufferObjID2;
	glGenVertexArrays(1, &vertexArrayObjID2);
	glBindVertexArray(vertexArrayObjID2);
	glGenBuffers(1, &vertexBufferObjID2);

	//VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID2);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), verticesRain, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(splashProgram, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(splashProgram, "inPosition"));

	LoadTGATextureSimple("./textures/splash.tga", &splash);
	glUniform1i(glGetUniformLocation(splashProgram, "tex"), 0);
	glUniform1i(glGetUniformLocation(splashProgram, "vertexMap"), 1);

	glUniformMatrix4fv(glGetUniformLocation(splashProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	glUniform3fv(glGetUniformLocation(splashProgram, "lightSourcesDirPosArr"), 5, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(splashProgram, "lightSourcesColorArr"), 5, &lightSourcesColorsArr[0].x);
	glUniform1f(glGetUniformLocation(splashProgram, "specularExponent"), specularExponent[0]);
	glUniform1iv(glGetUniformLocation(splashProgram, "isDirectional"), 5, isDirectional);
	glUniform3fv(glGetUniformLocation(splashProgram, "spotlightDirectionArr"), 5, &spotlightDirectionArr[0].x);
	glUniform1f(glGetUniformLocation(splashProgram, "spotlightCos"), spotlightCos[0]);
	glUniform1f(glGetUniformLocation(splashProgram, "specularExponent"), specExp);
	glUniform1f(glGetUniformLocation(splashProgram, "Kspecular"), Kspecular);
	glUniform1f(glGetUniformLocation(splashProgram, "Kdiffuse"), Kdiffuse);
	glUniform1f(glGetUniformLocation(splashProgram, "Kambient"), Kambient);

	printError("Init splash");


	glDisable(GL_BLEND);

	initRain(projectionMatrix);
}


void drawLamps(void)
{
	//Lamp
	mat4 modelToWorld = T(10, 0, 10);
	glUniformMatrix4fv(glGetUniformLocation(lampProgram, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);
	GLint isLight = 0;
	GLfloat Kambient = 1;
	GLfloat specExp = 5.0;
	GLfloat Kspecular = 1.0;
	GLfloat Kdiffuse = 0.1;
	glUniform1f(glGetUniformLocation(lampProgram, "specularExponent"), specExp);
	glUniform1f(glGetUniformLocation(lampProgram, "Kspecular"), Kspecular);
	glUniform1f(glGetUniformLocation(lampProgram, "Kdiffuse"), Kdiffuse);
	glUniform1f(glGetUniformLocation(lampProgram, "Kambient"), Kambient);

	//Draw them in a loop
	//mat4 trans = T(20, 0, 10);
	mat4 mtw = IdentityMatrix();
	int i = 0;
	for (i; i < 5; i++)
	{
		//Base
		mtw = Mult(T(i*40, 0, 0), modelToWorld);
		glUniformMatrix4fv(glGetUniformLocation(lampProgram, "modelToWorld"), 1, GL_TRUE, mtw.m);
		isLight = 0;
		glUniform1i(glGetUniformLocation(lampProgram, "isLight"), isLight);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, lampTex[0].texID);
		DrawModel(lamp[0], lampProgram, "inPosition", "inNormal", "inTexCoord");

		//Top
		glBindTexture(GL_TEXTURE_2D, lampTex[1].texID);
		DrawModel(lamp[1], lampProgram, "inPosition", "inNormal", "inTexCoord");

		//Light
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		isLight = 1;
		glUniform1i(glGetUniformLocation(lampProgram, "isLight"), isLight);

		glBindTexture(GL_TEXTURE_2D, lampTex[2].texID);
		DrawModel(lamp[2], lampProgram, "inPosition", "inNormal", "inTexCoord");

		printError("Draw lamp light");

		glDisable(GL_BLEND);
	} 

}

void drawRain(GLuint texture)
{
	t = t + 1;
	if (t % 10 == 0)
	{
		tSplash = tSplash + 1.0;
	}
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Splash
	glUseProgram(splashProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, splash);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1iv(glGetUniformLocation(splashProgram, "flipped"), 4, flipped);
	glUniform1f(glGetUniformLocation(splashProgram, "t"), tSplash);
	glBindVertexArray(vertexArrayObjID2);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 10000); //Instancing for particle system

	//Rain
	glUseProgram(rainProgram);
	glUniform1iv(glGetUniformLocation(rainProgram, "flipped"), 4, flipped);
	glUniform1f(glGetUniformLocation(rainProgram, "t"), tSplash);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, raindrop);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(vertexArrayObjID);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 150000); //Instancing for particle system

	printError("Draw rain");

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

//Function that updates position of rain
//by drawing to texture
//2 textures, one that is rendered to and (with new positions)
//one that is used to get the old positions. Switches 
void drawRain2(void)
{
	//glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	GLint viewport[4] = {0,0,0,0};
	glGetIntegerv(GL_VIEWPORT, viewport);
	if (renderTo == 2)
	{
		//If want to render to 2
		glBindFramebuffer(GL_FRAMEBUFFER, fb2);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, 512, 512);

		glUseProgram(texProgram);
		glUniform1f(glGetUniformLocation(texProgram, "t"), t);
		glActiveTexture(GL_TEXTURE1); //The position texture
		glBindTexture(GL_TEXTURE_2D, posTex1);
		glActiveTexture(GL_TEXTURE2); 
		glBindTexture(GL_TEXTURE_2D, speedTex);
		//Here I need to draw the quad.
		DrawModel(quad, texProgram, "inPosition", NULL, NULL);

		//Test
		//SaveFramebufferToTGA("./textures/postex2.tga", 0, 0, 512, 512);

		glUseProgram(0); //Do I need this?
		glBindFramebuffer(GL_FRAMEBUFFER, 0); //Unbind framebuffter. Important!
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		glUseProgram(rainProgram);
		drawRain(posTex2);
		renderTo = 1;
	} else
	{
		//If I want to render to 1

		glBindFramebuffer(GL_FRAMEBUFFER, fb1);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, 512, 512);
		//glViewport(0, 0, 1600, 1200);

		glUseProgram(texProgram);
		glUniform1f(glGetUniformLocation(texProgram, "t"), t);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, posTex2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, speedTex);

		DrawModel(quad, texProgram, "inPosition", NULL, NULL);
		//Test
		//SaveFramebufferToTGA("./textures/postex1.tga", 0, 0, 512, 512);
		glUseProgram(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glDrawBuffer(GL_BACK);
		//glBindFramebuffer(GL_FRAMEBUFFER, currentfbo);
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		glUseProgram(rainProgram);
		drawRain(posTex1);
		renderTo = 2;
	}
	printError("Draw rain 2");
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void room2Display(mat4 worldToView)
{
    mat4 flip;
    if (flipped[2])
	{
		flip = Ry(M_PI);
	}
	else
	{
		flip = IdentityMatrix();
	}

    glUseProgram(ground2Program);
    glUniformMatrix4fv(glGetUniformLocation(ground2Program, "worldToView"), 1, GL_TRUE, worldToView.m);
    glUniformMatrix4fv(glGetUniformLocation(ground2Program, "flip"), 1, GL_TRUE, flip.m);

	//Ground
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, grassTex);
	//Enviroment mapping for wet effect
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mapTex);
	mat4 modelToWorld = T(0, 100, 0);
	glUniformMatrix4fv(glGetUniformLocation(ground2Program, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);
	GLfloat specExp = 0.2;
	GLfloat Kspecular = 5;
	GLfloat Kdiffuse = 0.8;
	glUniform1f(glGetUniformLocation(ground2Program, "specularExponent"), specExp);
	glUniform1f(glGetUniformLocation(ground2Program, "Kspecular"), Kspecular);
	glUniform1f(glGetUniformLocation(ground2Program, "Kdiffuse"), Kdiffuse);
    DrawModel(ground, ground2Program, "inPosition", "inNormal", "inTexCoord");


	modelToWorld = T(-10, 20, 30);
	glUniformMatrix4fv(glGetUniformLocation(ground2Program, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);

	glUseProgram(lampProgram);
	glUniformMatrix4fv(glGetUniformLocation(lampProgram, "worldToView"), 1, GL_TRUE, worldToView.m);
    glUniformMatrix4fv(glGetUniformLocation(lampProgram, "flip"), 1, GL_TRUE, flip.m);
	drawLamps();

	//Rain
	glUseProgram(splashProgram);
	glUniformMatrix4fv(glGetUniformLocation(splashProgram, "worldToView"), 1, GL_TRUE, worldToView.m);
    glUniformMatrix4fv(glGetUniformLocation(splashProgram, "flip"), 1, GL_TRUE, flip.m);
	glUseProgram(rainProgram);
	glUniformMatrix4fv(glGetUniformLocation(rainProgram, "worldToView"), 1, GL_TRUE, worldToView.m);
    glUniformMatrix4fv(glGetUniformLocation(rainProgram, "flip"), 1, GL_TRUE, flip.m);
	glUseProgram(texProgram);
	glUniformMatrix4fv(glGetUniformLocation(texProgram, "worldToView"), 1, GL_TRUE, worldToView.m);
	drawRain2();
	glActiveTexture(GL_TEXTURE0);
}
