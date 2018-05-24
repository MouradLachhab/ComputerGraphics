#include "blob.h"

// Textures
GLuint rockyGround;

// Shader program
GLuint room3program, wellProgram, groundProgram, blobProgram;

// Objects
Model *groundFirstHalf, *groundSecondHalf, *well; //, *monster;
extern Model *door, *wall, *blob;
extern float currentDoorRy;
extern int mainRoom;
extern GLint flipped[];
extern vec3 wellPos, monsterPos, monsterDir, p, l;
extern float speed;


// Basic Matrices
extern mat4 projectionMatrix, modelToWorld, worldToView;

// Object transformation matrices
mat4 identityM, doorT, wellT, flip, monsterT, monsterR;

// Light sources
Point3D lightColor3[] = {{0.2f, 0.15f, 0.2f}, {1.0f, 1.0f, 1.0f}};
GLint isDirectional3[] = {0,1};
Point3D lightPositions3[] = {
		{60.0f, 60.0f, 60.0f},
		{-100.0f, 5.0f, -100.0f}
};
GLfloat specularExponent3[] = {20.0, 20.0};
Point3D wellPosition[]= {{30.0f, 0.0f, 0.0f}};

void initRoom3() {
	initBlob();
	// Load Objects
	groundFirstHalf = LoadModelPlus("./objects/floor.obj");
	groundSecondHalf = 	LoadModelPlus("./objects/floor2.obj");
	well = LoadModelPlus("./objects/well/old_well.obj");
	//monster = LoadModelPlus("./objects/cubeplus.obj");
	// Load Textures
	LoadTGATextureSimple("./textures/stone5_b.tga", &rockyGround);


	wellProgram = loadShaders("well.vert", "well.frag");
	glUseProgram(wellProgram);

	glUniformMatrix4fv(glGetUniformLocation(wellProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	// Light components
	glUniform3fv(glGetUniformLocation(wellProgram, "lightPositions"), 2, &lightPositions3[0].x);
	glUniform3fv(glGetUniformLocation(wellProgram, "lightColor"), 2, &lightColor3[0].x);
	glUniform1fv(glGetUniformLocation(wellProgram, "specularExponent"), 2, specularExponent3);
	glUniform1iv(glGetUniformLocation(wellProgram, "isDirectional"), 2, isDirectional3);


	groundProgram = loadShaders("ground.vert", "ground.frag");
	glUseProgram(groundProgram);
	glUniformMatrix4fv(glGetUniformLocation(groundProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform3fv(glGetUniformLocation(groundProgram, "wellPos"), 1, &wellPosition[0].x);

	// Light components
	glUniform3fv(glGetUniformLocation(groundProgram, "lightPositions"), 2, &lightPositions3[0].x);
	glUniform3fv(glGetUniformLocation(groundProgram, "lightColor"), 2, &lightColor3[0].x);
	glUniform1fv(glGetUniformLocation(groundProgram, "specularExponent"), 2, specularExponent3);
	glUniform1iv(glGetUniformLocation(groundProgram, "isDirectional"), 2, isDirectional3);


	room3program = loadShaders("room3.vert", "room3.frag");
	glUseProgram(room3program);
	glUniformMatrix4fv(glGetUniformLocation(room3program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	// Light components
	glUniform3fv(glGetUniformLocation(room3program, "lightPositions"), 2, &lightPositions3[0].x);
	glUniform3fv(glGetUniformLocation(room3program, "lightColor"), 2, &lightColor3[0].x);
	glUniform1fv(glGetUniformLocation(room3program, "specularExponent"), 2, specularExponent3);
	glUniform1iv(glGetUniformLocation(room3program, "isDirectional"), 2, isDirectional3);

	blobProgram = loadShaders("blob.vert", "blob.frag");
	glUniformMatrix4fv(glGetUniformLocation(blobProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);


	// Initialize transformation matrices
	identityM = IdentityMatrix();
	wellT = T(30, 0, 0);
}

void driveRoom3()
{
	vec3 f;
	f.x = wellPos.x - p.x;
	f.z = wellPos.z - p.z;
	f.y = 0;
	if(flipped[3])
	{
		f.x = -wellPos.x-p.x;
		f.z = -wellPos.z - p.z;
	}
	if (Norm(f)<5)
	{
		f = Normalize(f);
		p.x = p.x - f.x*speed/1.1;
		p.z = p.z - f.z*speed/1.1;
		l.x = l.x - f.x*speed/1.1;
		l.z = l.z - f.z*speed/1.1;
	}
	else
	{
		f = Normalize(f);
		p.x = p.x + f.x*speed/10;
		p.z = p.z + f.z*speed/10;
		l.x = l.x + f.x*speed/10;
		l.z = l.z + f.z*speed/10;
	}
	if(monsterPos.y < 6.2)
	{
	monsterPos.y = monsterPos.y + speed/30;
	}
	else
	{
		monsterDir.x = p.x - monsterPos.x;
		monsterDir.z = p.z - monsterPos.z;
		monsterDir.y = 0;
		if (flipped[3])
		{
			monsterDir.x = -p.x - monsterPos.x;
			monsterDir.z = -p.z - monsterPos.z;
		}
		if(Norm(monsterDir)<1)
		{
			exit(0);
		}
		else
		{
			monsterDir = Normalize(monsterDir);
			monsterPos.x = monsterPos.x + monsterDir.x*speed/10;
			monsterPos.z = monsterPos.z + monsterDir.z*speed/10;
		}
	}
}

void drawModels3() {
	updateMC();
	if (flipped[3])
	{
		flip = Ry(M_PI);
	}
	else
	{
		flip = IdentityMatrix();
	}

	// Variables Used
	mat4 transformation;
	GLfloat time = (GLfloat)glutGet(GLUT_ELAPSED_TIME);							// Get Current Time
	// Ground
	glUseProgram(groundProgram);
	glUniformMatrix4fv(glGetUniformLocation(groundProgram, "worldToView"), 1, GL_TRUE, worldToView.m);
	glUniformMatrix4fv(glGetUniformLocation(groundProgram, "flip"), 1, GL_TRUE, flip.m);
	glUniformMatrix4fv(glGetUniformLocation(groundProgram, "modelToWorld"), 1, GL_TRUE, identityM.m);
	glUniform1f(glGetUniformLocation(groundProgram, "angle"), time/100);
	DrawModel(groundFirstHalf, groundProgram, "inPosition", "inNormal", "inTexCoord");
	DrawModel(groundSecondHalf, groundProgram, "inPosition", "inNormal", "inTexCoord");


	// Well
	glUseProgram(wellProgram);
	glUniformMatrix4fv(glGetUniformLocation(wellProgram, "worldToView"), 1, GL_TRUE, worldToView.m);
	glUniformMatrix4fv(glGetUniformLocation(wellProgram, "flip"), 1, GL_TRUE, flip.m);
	glBindTexture(GL_TEXTURE_2D, rockyGround);
	transformation = wellT;
	glUniformMatrix4fv(glGetUniformLocation(wellProgram, "modelToWorld"), 1, GL_TRUE, transformation.m);
	glUniform1f(glGetUniformLocation(wellProgram, "angle"), time/100);
	glUniform3fv(glGetUniformLocation(wellProgram, "wellPos"), 0, &wellPosition[0].x);
	DrawModel(well, wellProgram,"inPosition", "inNormal", "inTexCoord");

	//monsterT = T(monsterPos.x, monsterPos.y, monsterPos.z);
	//transformation = Ry(monsterDir.y);
	//transformation = Mult(monsterT, transformation);

	//glUniformMatrix4fv(glGetUniformLocation(wellProgram, "modelToWorld"), 1, GL_TRUE, transformation.m);
	//DrawModel(monster, wellProgram,"inPosition", "inNormal", "inTexCoord");

	// Blob
	glUseProgram(blobProgram);
	glUniformMatrix4fv(glGetUniformLocation(blobProgram, "worldToView"), 1, GL_TRUE, worldToView.m);
	glUniformMatrix4fv(glGetUniformLocation(blobProgram, "flip"), 1, GL_TRUE, flip.m);
	monsterT = T(monsterPos.x, monsterPos.y, monsterPos.z);
	transformation = Ry(monsterDir.y);
	transformation = Mult(monsterT, transformation);
	glUniformMatrix4fv(glGetUniformLocation(blobProgram, "modelToWorld"), 1, GL_TRUE, transformation.m);
	DrawModel(blob, blobProgram,"inPosition", NULL, NULL);
}


void drawRoom3() {
	drawModels3();

}
