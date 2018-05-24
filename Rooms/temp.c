#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "skybox.h"
#include "room1.h"
#include "room2.h"
#include "room3.h"

GLuint program, programNonReflective, skyboxprogram;
mat4 projectionMatrix, modelToWorld;

//Objects
Model *wall, *sphere, *groundFirstHalf, *groundSecondHalf;

// Textures
GLuint skytex;

//Window size and cursor coordinates
GLfloat height = 1200.0;
GLfloat width = 1200.0;
GLfloat prevx = 0;
GLfloat prevy = 0;

// Variables for character movement
vec3 p,l,u;
mat4  worldToView;
float speed = 0.35;


Point3D starColor[] = {{1.0f, 1.0f, 1.0f},{0.37f, 0.37f, 0.37f}};
GLint isDirectional[] = {1,0};
GLint offset[] = {0};
Point3D lightPositions1[] = {
		{-100.0f, 5.0f, -100.0f},
		{50.0f, 5.0f, 0.0f}
};
Point3D lightPositions2[] = {
		{-100.0f, 5.0f, -100.0f},
		{50.0f, 5.0f, 0.0f}
};
GLfloat specularExponent[] = {20.0, 20.0};

void init(void)
{
	// Load objects
	wall = LoadModelPlus("./objects/test.obj");
	sphere = LoadModelPlus("./objects/groundsphere.obj");
	groundFirstHalf = LoadModelPlus("./objects/floor.obj");
	groundSecondHalf = 	LoadModelPlus("./objects/floor2.obj");

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 200.0);

	// Load and compile shader
	program = loadShaders("basic.vert", "basic.frag");
	glUseProgram(program);
	printError("init shader");

	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	// Star components
	glUniform3fv(glGetUniformLocation(program, "lightPositions"), 2, &lightPositions1[0].x);
	glUniform3fv(glGetUniformLocation(program, "starColor"), 2, &starColor[0].x);
	glUniform1fv(glGetUniformLocation(program, "specularExponent"), 2, specularExponent);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), 2, isDirectional);

// look at initial position
	p.x = 5;
	p.y = 5;
	p.z = 0;
	l.x = 0;
	l.y = 5;
	l.z = 0;

	int i = 0;
	for (i = 0; i < 6; i++)
	{
		box[i] = LoadDataToModel(
			vertices[i],
			NULL,
			texcoord[i],
			NULL,
			indices[i],
			4,
			6);
	}

	// Initialize skybox	
	skyboxprogram = loadShaders("skybox.vert", "skybox.frag");
	glUseProgram(skyboxprogram);
	glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
}

void characterMovement() {
	// Build matrix
	//right vector
	u.x = worldToView.m[0];
	u.y = worldToView.m[1];
	u.z = worldToView.m[2];

	//forward vector
	vec3 f;
	f.x = l.x-p.x;
	f.y = 0;
	f.z = l.z - p.z;
	f = Normalize(f);

	if (glutKeyIsDown(GLUT_KEY_W)) {
		p.x = p.x + f.x*speed;
		p.z = p.z + f.z*speed;
		l.x = l.x + f.x*speed;
		l.z = l.z + f.z*speed;
	}

	if (glutKeyIsDown(GLUT_KEY_S)) {
		p.x = p.x - f.x*speed;
		p.z = p.z - f.z*speed;
		l.x = l.x - f.x*speed;
		l.z = l.z - f.z*speed;
	}
	if (glutKeyIsDown(GLUT_KEY_D)) {
		p.x = p.x + u.x*speed;
		p.z = p.z + u.z*speed;
		l.x = l.x + u.x*speed;
		l.z = l.z + u.z*speed;
	}
	if (glutKeyIsDown(GLUT_KEY_A)) {
		p.x = p.x - u.x*speed;
		p.z = p.z - u.z*speed;
		l.x = l.x - u.x*speed;
		l.z = l.z - u.z*speed;
	}
	if (glutKeyIsDown(GLUT_STENCIL)) {
		p.y = p.y + speed;
		l.y = l.y + speed;
	}
	if (glutKeyIsDown(GLUT_KEY_LEFT_SHIFT)) {
		p.y = p.y - speed;
		l.y = l.y - speed;
	}
	if (glutKeyIsDown(GLUT_KEY_ESC)) {
		exit(0);
	}

	worldToView = lookAt(p.x, p.y, p.z,
		l.x, l.y, l.z,
		0, 1, 0);
}

// Skybox based on the demo
void drawSkybox() {

	int i;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glUseProgram(skyboxprogram);

	mat4 camMatrix = worldToView;
	camMatrix.m[3] = 0;
	camMatrix.m[7] = 0;
	camMatrix.m[11] = 0;
	glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "camMatrix"), 1, GL_TRUE, camMatrix.m);

	for (i = 0; i < 6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, t[i].texID);
		DrawModel(box[i], skyboxprogram, "inPosition", NULL, "inTexCoord");
	}
	glEnable(GL_DEPTH_TEST);
	glUseProgram(program);

}

void drawModels() {
	offset[0] = 0;
	glUniform1iv(glGetUniformLocation(program, "offset"), 1, offset);

	mat4 reflectionRotation = Rx(M_PI);
	// Wall
	glUniformMatrix4fv(glGetUniformLocation(program, "worldToView"), 1, GL_TRUE, worldToView.m);

	modelToWorld = IdentityMatrix();
	glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);

	DrawModel(wall, program, "inPosition", "inNormal", NULL);

	// Reflective Sphere
	mat4 translation = T(50, 5,0);

	glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorld"), 1, GL_TRUE, translation.m);
	DrawModel(sphere, program,"inPosition", "inNormal", NULL);

	// Wall reflected
	glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorld"), 1, GL_TRUE, reflectionRotation.m);

	DrawModel(wall, program, "inPosition", "inNormal", NULL);	

	// Reflected sphere
	translation = Mult(reflectionRotation, translation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorld"), 1, GL_TRUE, translation.m);
	DrawModel(sphere, program,"inPosition", "inNormal", NULL);


	// Reflective ground is drawn last
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);

	DrawModel(groundFirstHalf, program, "inPosition", "inNormal", NULL);
	
	offset[0] = 0;
	glUniform1iv(glGetUniformLocation(program, "offset"), 1, offset);
	DrawModel(groundSecondHalf, program, "inPosition", "inNormal", NULL);

	glDisable (GL_BLEND);
	/*
	mat4 translation = T(1, 0,100);
	mat4 scale = S(10.0f, 10.0f, 10.0f);
	translation = Mult(translation, scale);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorld"), 1, GL_TRUE, translation.m);
	DrawModel(testSphere, program, "inPosition", "inNormal", NULL);*/

}


void display(void)
{
	//printf("x: %f y: %f z: %f\n", (p.x), (p.y), (p.z));
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	printError("pre display");

	characterMovement();

	drawSkybox();

	drawModels();

	printError("display 2");

	glutSwapBuffers();
}

void timer(int i)
{
	glutTimerFunc(20, &timer, i);
	glutPostRedisplay();
}


float cameraDivider = 60;

void mouseInput(int x, int y) {

	vec3 v;
	mat3 worldToView3;

	v.x = x*1.0-prevx*1.0;
	v.y = prevy*1.0-y*1.0;
	v.z = 0;

	worldToView3 = mat4tomat3(worldToView);
	v = MultMat3Vec3(InvertMat3(worldToView3), v);

	l.x = l.x + v.x/cameraDivider;
	l.y = l.y + v.y/cameraDivider;
	l.z = l.z + v.z/cameraDivider;

	worldToView = lookAt(p.x, p.y, p.z,
		l.x, l.y, l.z,
		0, 1, 0);

	prevx = x;
	prevy = y;


	prevx = width / 2;
	prevy = height / 2;
	glutWarpPointer(prevx, prevy);
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (width,height);
	glutCreateWindow ("TSBK07 Lab 4");
	glutDisplayFunc(display);
	init ();
	loadTextures();
	glutHideCursor();
	glutTimerFunc(20, &timer, 0);
	glutPassiveMotionFunc(mouseInput);
	glutMainLoop();
	exit(0);
}
