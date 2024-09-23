//
// AVT: Phong Shading and Text rendered with FreeType library
// The text rendering was based on https://learnopengl.com/In-Practice/Text-Rendering
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
// 
// Author: Jo�o Madeiras Pereira
//

#include <math.h>
#include <iostream>
#include <sstream>
#include <string>

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>


// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

#include <IL/il.h>


// Use Very Simple Libs
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "VertexAttrDef.h"
#include "camera.h"
#include "Scenegraph.hpp"
#include "boat.h"

#include "avtFreeType.h"

using namespace std;

#define CAPTION "AVT Demo: Phong Shading and Text rendered with FreeType"
int WindowHandle = 0;
int WinX = 1024, WinY = 768;

unsigned int FrameCount = 0;

//shaders
VSShaderLib shader;  //geometry
VSShaderLib shaderText;  //render bitmap text

Scenegraph scenegraph;

//File with the font
const string font_name = "fonts/arial.ttf";

//Vector with meshes
Boat boat;

//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

GLint pvm_uniformId;
GLint vm_uniformId;
GLint normal_uniformId;
GLint lPos_uniformId;
GLint tex_loc, tex_loc1, tex_loc2;
	
// Cameras
int activeCamera = 0;
Camera cameras[3];
float ratio = (1.0f * WinX) / WinY;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[32];
float lightPos[4] = {4.0f, 6.0f, 2.0f, 1.0f};

// Scene Nodes
ScenegraphNode water_node;
ScenegraphNode ob1_node;
ScenegraphNode ob2_node;
ScenegraphNode ob3_node;
ScenegraphNode ob4_node;
ScenegraphNode ob5_node;
ScenegraphNode ob6_node;
ScenegraphNode house1_node;
ScenegraphNode house2_node;
ScenegraphNode boat_node;
ScenegraphNode boat_part1_node;
ScenegraphNode boat_part2_node;

// Scene Elements
SceneElement water_element;
SceneElement ob1_element;
SceneElement ob2_element;
SceneElement ob3_element;
SceneElement ob4_element;
SceneElement ob5_element;
SceneElement ob6_element;
SceneElement house1_element;
SceneElement house2_element;
SceneElement boat_element;
SceneElement boat_part1_element;
SceneElement boat_part2_element;


void timer(int value)
{
	std::ostringstream oss;
	oss << CAPTION << ": " << FrameCount << " FPS @ (" << WinX << "x" << WinY << ")";
	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
    FrameCount = 0;
    glutTimerFunc(1000, timer, 0);
}

void refresh(int value)
{
	float deltaTime = 1.0F / 60.0F;
	std::vector<float> movement{ 0.0F, 0.0F, 0.0F };

	for (int i = 0; i < boat_element.translation.size(); i++) {
		movement[i] = boat.speed * boat.direction[i] * deltaTime;
	}
	boat_node.move(movement);

	//myElements[boat.elementNum].translation += boat.direction * boat.speed * deltaTime;
	if (boat.speed > 0.0F) {
		boat.speed *= boat.speedDecay;
		//boat.speed = std::max(boat.speed - boat.speedDecay, 0.0F); // experiment boat.speed *= decay
	}

	cameras[activeCamera].target[0] = boat_element.translation[0];
	cameras[activeCamera].target[1] = boat_element.translation[1];
	cameras[activeCamera].target[2] = boat_element.translation[2];
	// boat.pos += boat.speed * boat.direction * deltaTime
	// if boat.speed > 0:
	//    boat.speed -= decay

	glutPostRedisplay();
	glutTimerFunc(1000/60, refresh, 0);
}

// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	if(h == 0)
		h = 1;
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// set the projection matrix
	// glGetInteger(GL_VIEWPORT, m_view)
	// float ratio = (m_view[2] - m_view[0))/(m_view[3] - m_view[1])
	ratio = (1.0f * w) / h;
	cameras[activeCamera].updateProjectionMatrix(ratio);
}


// ------------------------------------------------------------
//
// Render stufff
//

void renderScene(void) {

	FrameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	// set the camera using a function similar to gluLookAt
	vector<float> cameraPos = cameras[activeCamera].getPosition();
	lookAt(cameraPos[0], cameraPos[1], cameraPos[2], cameras[activeCamera].target[0], cameras[activeCamera].target[1], cameras[activeCamera].target[2], cameras[activeCamera].up[0], cameras[activeCamera].up[1], cameras[activeCamera].up[2]);

	// use our shader
	
	glUseProgram(shader.getProgramIndex());

		//send the light position in eye coordinates
		//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

		float res[4];
		multMatrixPoint(VIEW, lightPos,res);   //lightPos definido em World Coord so is converted to eye space
		glUniform4fv(lPos_uniformId, 1, res);


	scenegraph.draw();

	//Render text (bitmap fonts) in screen coordinates. So use ortoghonal projection with viewport coordinates.
	glDisable(GL_DEPTH_TEST);
	//the glyph contains transparent background colors and non-transparent for the actual character pixels. So we use the blending
	glEnable(GL_BLEND);  
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	//viewer at origin looking down at  negative z direction
	pushMatrix(MODEL);
	loadIdentity(MODEL);
	pushMatrix(PROJECTION);
	loadIdentity(PROJECTION);
	pushMatrix(VIEW);
	loadIdentity(VIEW);
	ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
	RenderText(shaderText, "This is a sample text", 25.0f, 25.0f, 1.0f, 0.5f, 0.8f, 0.2f);
	RenderText(shaderText, "AVT Light and Text Rendering Demo", 440.0f, 570.0f, 0.5f, 0.3, 0.7f, 0.9f);
	popMatrix(PROJECTION);
	popMatrix(VIEW);
	popMatrix(MODEL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glutSwapBuffers();
}

// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy)
{
	switch(key) {

		case 27:
			glutLeaveMainLoop();
			break;

		case 'c': 
			printf("Camera Spherical Coordinates (%f, %f, %f)\n", cameras[activeCamera].alpha, cameras[activeCamera].beta, cameras[activeCamera].distance);
			break;
		case 'm': glEnable(GL_MULTISAMPLE); break;
		case 'n': glDisable(GL_MULTISAMPLE); break;
		case '1': 
			activeCamera = 0;
			cameras[activeCamera].updateProjectionMatrix(ratio);
			break;
		case '2': 
			activeCamera = 1; 
			cameras[activeCamera].updateProjectionMatrix(ratio);
			break;
		case '3': 
			activeCamera = 2; 
			cameras[activeCamera].updateProjectionMatrix(ratio);
			break;
	}

	if (key == 'd') {
		boat.increaseSpeed(0.4F);
		std::vector<float> rotation = { -1.0F, 0, 0, 0 };
		boat_node.spin(rotation);
		boat.setDirection(boat_element.rotation);
		//rotate(direction, -1)
	}
	if (key == 'a') {
		boat.increaseSpeed(0.4F);
		std::vector<float> rotation = { 1.0F, 0, 0, 0 };
		boat_node.spin(rotation);
		boat.setDirection(boat_element.rotation);
		//boat.direction = rotate() * boat.direction;
		//rotate(direction, 1)
	}
}


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			cameras[activeCamera].alpha -= (xx - startX);
			cameras[activeCamera].beta += (yy - startY);
		}
		else if (tracking == 2) {
			cameras[activeCamera].distance += (yy - startY) * 0.01f;
			if (cameras[activeCamera].distance < 0.1f)
				cameras[activeCamera].distance = 0.1f;
		}
		tracking = 0;
	}
}

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX =  - xx + startX;
	deltaY =    yy - startY;

	// left mouse button: move camera
	if (tracking == 1) {


		alphaAux = cameras[activeCamera].alpha + deltaX;
		betaAux = cameras[activeCamera].beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;
		rAux = cameras[activeCamera].distance;
	}
	// right mouse button: zoom
	else if (tracking == 2) {

		alphaAux = cameras[activeCamera].alpha;
		betaAux = cameras[activeCamera].beta;
		rAux = cameras[activeCamera].distance + (deltaY * 0.01f);
		if (rAux < 0.1f)
			rAux = 0.1f;
	}

	if (activeCamera == 0) {
		cameras[activeCamera].distance = rAux;
		cameras[activeCamera].setOffset(alphaAux, betaAux);
	}
	else if (activeCamera == 1) {
		cameras[activeCamera].distance = rAux;
		cameras[activeCamera].setOffset();
	}


//  uncomment this if not using an idle or refresh func
//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {
	if (activeCamera == 2) {
		return;
	}

	cameras[activeCamera].distance += direction * 0.1f;
	if (cameras[activeCamera].distance < 0.1f)
		cameras[activeCamera].distance = 0.1f;

	cameras[activeCamera].setOffset();

//  uncomment this if not using an idle or refresh func
//	glutPostRedisplay();
}

// --------------------------------------------------------
//
// Shader Stuff
//


GLuint setupShaders() {

	// Shader for models
	shader.init();
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/pointlight_gourand.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/pointlight_gourand.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shader.getProgramIndex(), 0,"colorOut");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	//glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shader.getProgramIndex());
	printf("InfoLog for Model Rendering Shader\n%s\n\n", shaderText.getAllInfoLogs().c_str());

	if (!shader.isProgramValid()) {
		printf("GLSL Model Program Not Valid!\n");
		exit(1);
	}

	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texmap2");
	
	printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shader.getAllInfoLogs().c_str());

	// Shader for bitmap Text
	shaderText.init();
	shaderText.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/text.vert");
	shaderText.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/text.frag");

	glLinkProgram(shaderText.getProgramIndex());
	printf("InfoLog for Text Rendering Shader\n%s\n\n", shaderText.getAllInfoLogs().c_str());

	if (!shaderText.isProgramValid()) {
		printf("GLSL Text Program Not Valid!\n");
		exit(1);
	}
	
	return(shader.isProgramLinked() && shaderText.isProgramLinked());
}

// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

void initMap()
{
	
	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	int texcount = 0;

	float amb1[] = { 0.0f, 0.3f, 1.0f, 1.0f };
	float diff1[] = { 0.1f, 0.1f, 0.8f, 1.0f };
	float spec1[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float shininess = 500.0;
	
	// Water -------------------------------------------
	water_element.mesh = createQuad(160.0F, 160.0F);
	memcpy(water_element.mesh.mat.ambient, amb1, 4 * sizeof(float));
	memcpy(water_element.mesh.mat.diffuse, diff1, 4 * sizeof(float));
	memcpy(water_element.mesh.mat.specular, spec1, 4 * sizeof(float));
	memcpy(water_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	water_element.mesh.mat.shininess = shininess;
	water_element.mesh.mat.texCount = texcount;
	water_element.translation = { 0.0F, 0.0F, 0.0F };
	water_element.rotation = { -90.0F, 1.0F, 0.0F, 0.0F };
	water_node = ScenegraphNode(1, &water_element, &shader);
	scenegraph.addNode(&water_node);
	
	// Obstacles ----------------------------------------
	shininess = 100.0f;

	ob1_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob1_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(ob1_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(ob1_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(ob1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob1_element.mesh.mat.shininess = shininess;
	ob1_element.mesh.mat.texCount = texcount;
	ob1_element.translation = { -1.0F, 0.0F, 0.0F }; //Starting position
	ob1_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob1_node = ScenegraphNode(2, &ob1_element, &shader);
	scenegraph.addNode(&ob1_node);

	ob2_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob2_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(ob2_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(ob2_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(ob2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob2_element.mesh.mat.shininess = shininess;
	ob2_element.mesh.mat.texCount = texcount;
	ob2_element.translation = { -1.0F, 0.0F, 5.0F }; //Starting position
	ob2_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob2_node = ScenegraphNode(3, &ob2_element, &shader);
	scenegraph.addNode(&ob2_node);

	ob3_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob3_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(ob3_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(ob3_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(ob3_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob3_element.mesh.mat.shininess = shininess;
	ob3_element.mesh.mat.texCount = texcount;
	ob3_element.translation = { 5.0F, 0.0F, 2.0F }; //Starting position
	ob3_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob3_node = ScenegraphNode(4, &ob3_element, &shader);
	scenegraph.addNode(&ob3_node);

	ob4_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob4_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(ob4_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(ob4_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(ob4_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob4_element.mesh.mat.shininess = shininess;
	ob4_element.mesh.mat.texCount = texcount;
	ob4_element.translation = { 13.0F, 0.0F, 0.0F }; //Starting position
	ob4_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob4_node = ScenegraphNode(5, &ob4_element, &shader);
	scenegraph.addNode(&ob4_node);

	ob5_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob5_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(ob5_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(ob5_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(ob5_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob5_element.mesh.mat.shininess = shininess;
	ob5_element.mesh.mat.texCount = texcount;
	ob5_element.translation = { 12.0F, 0.0F, 5.0F }; //Starting position
	ob5_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob5_node = ScenegraphNode(6, &ob5_element, &shader);
	scenegraph.addNode(&ob5_node);

	ob6_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob6_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(ob6_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(ob6_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(ob6_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob6_element.mesh.mat.shininess = shininess;
	ob6_element.mesh.mat.texCount = texcount;
	ob6_element.translation = { 5.0F, 0.0F, -3.0F }; //Starting position
	ob6_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob6_node = ScenegraphNode(7, &ob6_element, &shader);
	scenegraph.addNode(&ob6_node);


	// Houses ------------------------------------------
	house1_element.mesh = createCube();
	memcpy(house1_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(house1_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(house1_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(house1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	house1_element.mesh.mat.shininess = shininess;
	house1_element.mesh.mat.texCount = texcount;
	house1_element.translation = { 3.0F, 0.0F, 7.0F }; //Starting position
	house1_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	house1_node = ScenegraphNode(8, &house1_element, &shader);
	scenegraph.addNode(&house1_node);
	
	house2_element.mesh = createCube();
	memcpy(house2_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(house2_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(house2_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(house2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	house2_element.mesh.mat.shininess = shininess;
	house2_element.mesh.mat.texCount = texcount;
	house2_element.translation = { 10.0F, 0.0F, -9.0F }; //Starting position
	house2_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	house2_node = ScenegraphNode(9, &house2_element, &shader);
	scenegraph.addNode(&house2_node);
}

void initBoat() {
	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	int texcount = 0;
	float shininess = 100.0f;

	// Boat as a whole
	boat_element.translation = { 0.0F, 0.0F, 0.0F }; //Starting position
	boat_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	boat_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	boat_node = ScenegraphNode(0, &boat_element, &shader);
	scenegraph.addNode(&boat_node);

	// Boat Body
	boat_part2_element.mesh = createCube();
	memcpy(boat_part2_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(boat_part2_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(boat_part2_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(boat_part2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	boat_part2_element.mesh.mat.shininess = shininess;
	boat_part2_element.mesh.mat.texCount = texcount;
	boat_part2_element.translation = { 0.0F, 0.0F, 0.0F }; //Starting position
	boat_part2_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	boat_part2_element.scale = { 1.0F, 0.5F, 2.0F, 0.0F};
	boat_part2_node = ScenegraphNode(0, &boat_part2_element, &shader);
	boat_node.addNode(&boat_part2_node);

	// Boad Head
	boat_part1_element.mesh = createCube();
	memcpy(boat_part1_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(boat_part1_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(boat_part1_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(boat_part1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	boat_part1_element.mesh.mat.shininess = shininess;
	boat_part1_element.mesh.mat.texCount = texcount;
	boat_part1_element.translation = { 0.0F, 0.0F, 2.0F }; //Starting position
	boat_part1_element.rotation = { 45.0F, 0.0F, 1.0F, 0.0F };
	boat_part1_element.scale = { (float)sqrt(0.5), 0.5F, (float)sqrt(0.5), 0.0F};
	boat_part1_node = ScenegraphNode(0, &boat_part1_element, &shader);
	boat_node.addNode(&boat_part1_node);
}

void init()
{
	SceneElement element;

	/* Initialization of DevIL */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		printf("wrong DevIL version \n");
		exit(0);
	}
	ilInit();

	/// Initialization of freetype library with font_name file
	freeType_init(font_name);

	cameras[0] = Camera(0, 10.0F, 39.0F, 51.0F);
	cameras[1] = Camera(0, 20.0F, 0.0F, 90.0F);
	cameras[2] = Camera(1, 20.0F, 0.0F, 90.0F);
	
	// create geometry and VAO of the pawn
	/*
	element.mesh = createPawn();
	memcpy(element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	element.mesh.mat.shininess = shininess;
	element.mesh.mat.texCount = texcount;
	element.translation = { 0, 0, 0 }; //Starting position
	myElements.push_back(element);

	
	// create geometry and VAO of the sphere
	element.mesh = createSphere(1.0f, 20);
	memcpy(element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	element.mesh.mat.shininess = shininess;
	element.mesh.mat.texCount = texcount;
	element.translation = { 2, 0, 1 }; //Starting position
	myElements.push_back(element);
	*/
	
	/*
	// create geometry and VAO of the 
	element.mesh = createCone(1.5f, 0.5f, 20);
	memcpy(element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	element.mesh.mat.shininess = shininess;
	element.mesh.mat.texCount = texcount;
	element.translation = { 0, 0, 1 }; //Starting position
	myElements.push_back(element);
	*/

	initBoat();
	initMap();

	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char **argv) {

//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE);

	glutInitContextVersion (4, 3);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(WinX, WinY);
	WindowHandle = glutCreateWindow(CAPTION);


//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	glutTimerFunc(0, timer, 0);
	//glutIdleFunc(renderScene);  // Use it for maximum performance
	glutTimerFunc(0, refresh, 0);    //use it to to get 60 FPS whatever

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutMouseWheelFunc ( mouseWheel ) ;
	

//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
		return(1);

	init();

	//  GLUT main loop
	glutMainLoop();

	return(0);
}



