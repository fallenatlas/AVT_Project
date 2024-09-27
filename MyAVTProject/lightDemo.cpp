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
#include <cstdlib>

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
#include "aabb.h"
#include "Texture_Loader.h"

#include "avtFreeType.h"

using namespace std;

#define CAPTION "AVT Demo: Phong Shading and Text rendered with FreeType"
#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2
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
Boat monster1;
Boat monster2;

//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

GLint tex_loc, tex_loc1, tex_loc2;
GLuint TextureArray[3];
	
// Cameras
int activeCamera = 0;
Camera cameras[3];
float ratio = (1.0f * WinX) / WinY;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[32];

// Lights
//float lightPos[4] = {4.0f, 6.0f, 2.0f, 1.0f};

float directionalLightDir[4]{ 1.0f, 1000.0f, 1.0f, 0.0f };
float pointLightPos[NUM_POINT_LIGHTS][4]{
	{ 40.0F, 1.5F, -35.0F, 1.0F },
	{ 50.0F, 1.5F, -30.0F, 1.0F },
	{ 0.0F, 1.5F, 20.0F, 1.0F },
	{ 10.0F, 1.5F, 20.0F, 1.0F },
	{ -20.0F, 1.5F, 55.0F, 1.0F },
	{ -20.0F, 1.5F, 65.0F, 1.0F }
};
float spotLightPos[NUM_SPOT_LIGHTS][4]{
	{ 0.0F, 1.0F, 0.0F, 1.0F },
	{ 0.5F, 1.0F, 0.0F, 1.0F }
}; // need to be going with the car
float spotLightDir[NUM_SPOT_LIGHTS][4]{
	{ 0.0F, 0.0F, 1.0F, 0.0F },
	{ 0.0F, 0.0F, 1.0F, 0.0F }
};// need to be point in the direction of the car

bool dirLightActive = true; // 0 - off, 1 - on
bool pointLightsActive = true; // see if we can use bools or something
bool spotLightsActive = true;

// Toggles
bool key_a_is_pressed = false;
bool key_d_is_pressed = false;
bool key_s_is_pressed = false;

// Timer
float monster_speed_timer = 0;
const float RESPAWN_TIME = 5.0F;

// Textures
const int NO_TEXTURE = 0;
const int WOOD_TEXTURE = 1;

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
ScenegraphNode left_paddle_node;
ScenegraphNode left_paddle_part1_node;
ScenegraphNode left_paddle_part2_node;
ScenegraphNode right_paddle_node;
ScenegraphNode right_paddle_part1_node;
ScenegraphNode right_paddle_part2_node;
ScenegraphNode monster1_node;
ScenegraphNode monster1_part1_node;
ScenegraphNode monster1_part2_node;
ScenegraphNode monster1_part3_node;
ScenegraphNode monster1_part4_node;
ScenegraphNode monster1_part5_node;
ScenegraphNode monster1_part6_node;
ScenegraphNode monster2_node;
ScenegraphNode monster2_part1_node;
ScenegraphNode monster2_part2_node;
ScenegraphNode monster2_part3_node;
ScenegraphNode monster2_part4_node;
ScenegraphNode monster2_part5_node;
ScenegraphNode monster2_part6_node;

// Islands
ScenegraphNode island1_node;
ScenegraphNode island2_node;
ScenegraphNode island3_node;
ScenegraphNode island4_node;

ScenegraphNode debug1_node;
ScenegraphNode debug2_node;

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
SceneElement left_paddle_element;
SceneElement left_paddle_part1_element;
SceneElement left_paddle_part2_element;
SceneElement right_paddle_element;
SceneElement right_paddle_part1_element;
SceneElement right_paddle_part2_element;
SceneElement monster1_element;
SceneElement monster1_part1_element;
SceneElement monster1_part2_element;
SceneElement monster1_part3_element;
SceneElement monster1_part4_element;
SceneElement monster1_part5_element;
SceneElement monster1_part6_element;
SceneElement monster2_element;
SceneElement monster2_part1_element;
SceneElement monster2_part2_element;
SceneElement monster2_part3_element;
SceneElement monster2_part4_element;
SceneElement monster2_part5_element;
SceneElement monster2_part6_element;

// Islands
SceneElement island1_element;
SceneElement island2_element;
SceneElement island3_element;
SceneElement island4_element;

SceneElement debug1_element;
SceneElement debug2_element;

// AABBs
AABB boat_aabb;

//bool update_static_aabbs = true;
AABB island1_aabb;
AABB island2_aabb;
AABB island3_aabb;
AABB island4_aabb;

const std::vector<float> initialBoatPos = { 65.0F, 0.0F, -70.0F };
const std::vector<float> initialBoatRot = { 0.0F, 0.0F, 1.0F, 0.0F };

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

void collide_and_resolve(AABB& boat_aabb, AABB& other_aabb, SceneElement& other_element, bool respawn) {
	auto info = boat_aabb.intersepts(other_aabb);
	if (info.collided) {
		if (respawn) {
			// reset boat in initial position
			boat_element.translation = initialBoatPos;
			boat_element.rotation = initialBoatRot;
			boat.speed = 0;
			boat.setDirection(boat_element.rotation);
			cameras[0].boatAngle = boat_element.rotation[0];
			cameras[0].setOffset();
			return;
		}

		std::vector<float> pushDirection;
		if (info.axis == 0) {
			pushDirection = { 1.0f, 0.0f, 0.0f };
		}
		else if (info.axis == 2) {
			pushDirection = { 0.0F, 0.0F, 1.0F };
		}
		if (dotProduct(boat.direction.data(), pushDirection.data()) < 0.0f) {
			pushDirection[0] = -pushDirection[0];
			pushDirection[2] = -pushDirection[2];
		}

		other_element.translation = { other_element.translation[0] + pushDirection[0] * info.penetration * boat.speed,
									  other_element.translation[1],
									  other_element.translation[2] + pushDirection[2] * info.penetration * boat.speed };
		boat.speed = 0; // we might potentially need to change this to like velocity
	}
}

void handle_collisions() {
	// update the boat aabb
	std::vector<float> points1;
	AABB::getGlobalCubePoints(boat_part1_element.mesh.transform, points1);
	std::vector<float> points2;
	AABB::getGlobalCubePoints(boat_part2_element.mesh.transform, points2);
	points1.insert(points1.end(), points2.begin(), points2.end());

	boat_aabb.updateWithVec(points1);

	// update islands aabbs
	points1.clear();
	AABB::getGlobalCubePoints(island1_element.mesh.transform, points1);
	island1_aabb.updateWithVec(points1);

	points1.clear();
	AABB::getGlobalCubePoints(island2_element.mesh.transform, points1);
	island2_aabb.updateWithVec(points1);

	points1.clear();
	AABB::getGlobalCubePoints(island3_element.mesh.transform, points1);
	island3_aabb.updateWithVec(points1);

	points1.clear();
	AABB::getGlobalCubePoints(island4_element.mesh.transform, points1);
	island4_aabb.updateWithVec(points1);

	debug1_element.translation = island1_aabb.max;
	debug2_element.translation = island1_aabb.min;

	// update cylinder aabbs

	// update monters aabbs

	// test collision with test cube
	collide_and_resolve(boat_aabb, island1_aabb, island1_element, false);
	collide_and_resolve(boat_aabb, island2_aabb, island2_element, false);
	collide_and_resolve(boat_aabb, island3_aabb, island3_element, false);
	collide_and_resolve(boat_aabb, island3_aabb, island4_element, false);
	

	// if cube is enemy reset boat at start

	// update aabb for every monster (the aabbs for houses and stuff are static)
	//for (auto aabb : monster_aabbs) {
	//	aabb.update()
	//}

	// check for collisons between boat and every other object
	//for (auto aabb : obstacles) {
	//
	//}

}

bool paddle_is_in_the_water(SceneElement paddle) {
	return (paddle.rotation[0] >= 0 && ((int)paddle.rotation[0] % 360 < 55 || (int)paddle.rotation[0] % 360 > 310)) ||
		paddle.rotation[0] < 0 && ((int)paddle.rotation[0] % 360 > -50 || (int)paddle.rotation[0] % 360 < -305);
}

void haddle_movement() {
	int rotation_dir = 1;
	if (key_s_is_pressed)
		rotation_dir = -1;

	float deltaTime = 1.0F / 60.0F;
	std::vector<float> movement{ 0.0F, 0.0F, 0.0F };

	for (int i = 0; i < boat_element.translation.size(); i++) {
		movement[i] = boat.speed * boat.direction[i] * deltaTime;
	}
	boat_node.move(movement);

	//myElements[boat.elementNum].translation += boat.direction * boat.speed * deltaTime;
	if (boat.speed != 0.0F) {
		boat.speed *= boat.speedDecay;
		//boat.speed = std::max(boat.speed - boat.speedDecay, 0.0F); // experiment boat.speed *= decay
	}

	std::vector<float> rotation = { rotation_dir * boat.rowing_speed, 0, 0, 0 };

	if (key_a_is_pressed) {
		left_paddle_node.spin(rotation);
		if (paddle_is_in_the_water(left_paddle_element)) {
			boat.increaseSpeed(boat.acceleration * rotation_dir);
			rotation = { 1.0F * rotation_dir, 0, 0, 0 };
			boat_node.spin(rotation);
			boat.setDirection(boat_element.rotation);
			cameras[0].boatAngle = boat_element.rotation[0];
			cameras[0].setOffset();
		}
	}
	rotation = { rotation_dir * boat.rowing_speed, 0, 0, 0 };

	if (key_d_is_pressed) {
		right_paddle_node.spin(rotation);
		if (paddle_is_in_the_water(right_paddle_element)) {
			boat.increaseSpeed(boat.acceleration * rotation_dir);
			rotation = { -1.0F * rotation_dir, 0, 0, 0 };
			boat_node.spin(rotation);
			boat.setDirection(boat_element.rotation);
			cameras[0].boatAngle = boat_element.rotation[0];
			cameras[0].setOffset();
		}
	}
}

void respawn_monster(ScenegraphNode* monster_node, Boat* monster) {
	int index = rand() % 4;
	vector<float> respawn_position = { 0.0, 0.5, 0.0 };
	vector<float> respawn_rotation = { 0.0, 0.0, 1.0, 0.0};

	switch (index) {
		case 0:
			respawn_position[0] = 80.0F;
			respawn_position[2] = rand() % 160 - 80;
			respawn_rotation[0] = rand() % 45;
			break;
		case 1:
			respawn_position[0] = -80.0F;
			respawn_position[2] = rand() % 160 - 80;
			respawn_rotation[0] = rand() % 45 + 180;
			break;
		case 2:
			respawn_position[0] = rand() % 160 - 80;
			respawn_position[2] = 80.0F;
			respawn_rotation[0] = rand() % 45 + 270;
			break;
		case 3:
			respawn_position[0] = rand() % 160 - 80;
			respawn_position[2] = -80.0F;
			respawn_rotation[0] = rand() % 45 + 90;
			break;
	}

	monster_node->position(respawn_position);
	monster_node->setRotation(respawn_rotation);
	monster->initRotation = respawn_rotation[0];
	respawn_rotation[0] -= 90; //Because the model is sideways... ops
	monster->setDirection(respawn_rotation);
}

void haddle_monster_movement() {
	float deltaTime = 1.0F / 60.0F;
	std::vector<float> movement1{ 0.0F, 0.0F, 0.0F };
	std::vector<float> movement2{ 0.0F, 0.0F, 0.0F };

	// Monster respawn timer
	if (monster1.respawnTimer > 0)
		monster1.respawnTimer -= deltaTime;
	if (monster2.respawnTimer > 0)
		monster2.respawnTimer -= deltaTime;

	// Increase monster speed with game time
	monster_speed_timer += deltaTime;
	if (monster_speed_timer >= 5 && monster1.speed < monster1.maxSpeed) { // every 5 sec
		monster1.speed += 2;
		monster2.speed += 2;
		monster_speed_timer = 0;
	}

	// Monster wiggle
	if (monster1_element.rotation[0] > monster1.initRotation + 20 || monster1_element.rotation[0] < monster1.initRotation - 20)
		monster1.rotationDir *= -1;
	if (monster2_element.rotation[0] > monster2.initRotation + 20 || monster2_element.rotation[0] < monster2.initRotation - 20)
		monster2.rotationDir *= -1;
	std::vector<float> rotation = { monster1.rotationDir, 0, 0, 0 };
	monster1_node.spin(rotation);
	rotation = { monster2.rotationDir, 0, 0, 0 };
	monster2_node.spin(rotation);

	// Forward movement
	for (int i = 0; i < monster1_element.translation.size(); i++) {
		movement1[i] = monster1.speed * monster1.direction[i] * deltaTime;
		movement2[i] = monster2.speed * monster2.direction[i] * deltaTime;
	}
	monster1_node.move(movement1);
	monster2_node.move(movement2);

	// Out of bounds
	if ((monster1_element.translation[0] > 80 || monster1_element.translation[0] < -80 ||
		monster1_element.translation[2] > 80 || monster1_element.translation[2] < -80) &&
		monster1_node.isActive == true) {
		monster1_node.isActive = false;
		monster1.respawnTimer = RESPAWN_TIME;
	}
	if ((monster2_element.translation[0] > 80 || monster2_element.translation[0] < -80 ||
		monster2_element.translation[2] > 80 || monster2_element.translation[2] < -80) &&
		monster2_node.isActive == true) {
		monster2_node.isActive = false;
		monster2.respawnTimer = RESPAWN_TIME;
	}

	// Respawn
	if (!monster1_node.isActive && monster1.respawnTimer <= 0) {
		monster1_node.isActive = true;
		respawn_monster(&monster1_node, &monster1);
	}
	if (!monster2_node.isActive && monster2.respawnTimer <= 0) {
		monster2_node.isActive = true;
		respawn_monster(&monster2_node, &monster2);
	}
}

void refresh(int value)
{
	handle_collisions();

	haddle_movement();

	haddle_monster_movement();

	cameras[activeCamera].target[0] = boat_element.translation[0];
	cameras[activeCamera].target[1] = boat_element.translation[1];
	cameras[activeCamera].target[2] = boat_element.translation[2];

	for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
		// modify per thingy
		float perpVector[3] = { boat.direction[2], 0.0F, -boat.direction[0] };
		float modifier = -0.3f;
		if (i == 1) {
			modifier = 0.3f;
		}

		for (int j = 0; j < 3; j++) {
			spotLightPos[i][j] = boat_element.translation[j] + boat.direction[j] * -0.0F + perpVector[j] * modifier;
		}
		spotLightPos[i][1] += 1.0F;
		spotLightPos[i][3] = 1.0f;
	}

	for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
		for (int j = 0; j < 3; j++) {
			spotLightDir[i][j] = boat.direction[j];
		}
		spotLightDir[i][1] -= 0.25f;
		spotLightDir[i][3] = 0.0f;
	}
	

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

	GLint loc;

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
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[0]);  

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureArray[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TextureArray[2]);

	//Indicar aos tres samplers do GLSL quais os Texture Units a serem usados
	glUniform1i(tex_loc, 0);  
	glUniform1i(tex_loc1, 1); 
	glUniform1i(tex_loc2, 2); 

		//send the light position in eye coordinates
		//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

		float res[4];
		//multMatrixPoint(VIEW, lightPos,res);   //lightPos definido em World Coord so is converted to eye space
		//glUniform4fv(lPos_uniformId, 1, res);

		loc = glGetUniformLocation(shader.getProgramIndex(), "dirLightOn");
		glUniform1i(loc, dirLightActive ? 1 : 0);
		multMatrixPoint(VIEW, directionalLightDir, res);
		loc = glGetUniformLocation(shader.getProgramIndex(), "dirLight.direction");
		glUniform4fv(loc, 1, res);

		loc = glGetUniformLocation(shader.getProgramIndex(), "pointLightsOn");
		glUniform1i(loc, pointLightsActive ? 1 : 0);
		multMatrixPoint(VIEW, pointLightPos[0], res);
		loc = glGetUniformLocation(shader.getProgramIndex(), "pointLights[0].position");
		glUniform4fv(loc, 1, res);

		multMatrixPoint(VIEW, pointLightPos[1], res);
		loc = glGetUniformLocation(shader.getProgramIndex(), "pointLights[1].position");
		glUniform4fv(loc, 1, res);

		multMatrixPoint(VIEW, pointLightPos[2], res);
		loc = glGetUniformLocation(shader.getProgramIndex(), "pointLights[2].position");
		glUniform4fv(loc, 1, res);

		multMatrixPoint(VIEW, pointLightPos[3], res);
		loc = glGetUniformLocation(shader.getProgramIndex(), "pointLights[3].position");
		glUniform4fv(loc, 1, res);

		multMatrixPoint(VIEW, pointLightPos[4], res);
		loc = glGetUniformLocation(shader.getProgramIndex(), "pointLights[4].position");
		glUniform4fv(loc, 1, res);

		multMatrixPoint(VIEW, pointLightPos[5], res);
		loc = glGetUniformLocation(shader.getProgramIndex(), "pointLights[5].position");
		glUniform4fv(loc, 1, res);

		loc = glGetUniformLocation(shader.getProgramIndex(), "spotLightsOn");
		glUniform1i(loc, spotLightsActive ? 1 : 0);
		multMatrixPoint(VIEW, spotLightPos[0], res);
		loc = glGetUniformLocation(shader.getProgramIndex(), "spotLights[0].position");
		glUniform4fv(loc, 1, res);
		multMatrixPoint(VIEW, spotLightDir[0], res);
		loc = glGetUniformLocation(shader.getProgramIndex(), "spotLights[0].direction");
		glUniform4fv(loc, 1, res);
		loc = glGetUniformLocation(shader.getProgramIndex(), "spotLights[0].cutOff");
		glUniform1f(loc, cos(12.5f * 3.14f / 180.0f));

		multMatrixPoint(VIEW, spotLightPos[1], res);
		loc = glGetUniformLocation(shader.getProgramIndex(), "spotLights[1].position");
		glUniform4fv(loc, 1, res);
		multMatrixPoint(VIEW, spotLightDir[0], res);
		loc = glGetUniformLocation(shader.getProgramIndex(), "spotLights[1].direction");
		glUniform4fv(loc, 1, res);
		loc = glGetUniformLocation(shader.getProgramIndex(), "spotLights[1].cutOff");
		glUniform1f(loc, cos(12.5f * 3.14f / 180.0f));


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
		/*
		case 'c': 
			printf("Camera Spherical Coordinates (%f, %f, %f)\n", cameras[activeCamera].alpha, cameras[activeCamera].beta, cameras[activeCamera].distance);
			break;
		case 'm': glEnable(GL_MULTISAMPLE); break;
		case 'n': glDisable(GL_MULTISAMPLE); break;
		*/
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
		case 'n':
			dirLightActive = !dirLightActive;
			break;
		case 'c':
			pointLightsActive = !pointLightsActive;
			break;
		case 'h':
			spotLightsActive = !spotLightsActive;
			break;
		case 'a':
			key_a_is_pressed = true;
			break;
		case 'd':
			key_d_is_pressed = true;
			break;
		case 's':
			key_s_is_pressed = true;
			break;
		case 'o':
			boat.acceleration = 0.6;
			boat.rowing_speed = 15.0;
			break;

	}
}

void processUpKeys(unsigned char key, int xx, int yy)
{
	switch (key) {
		case 'a':
			key_a_is_pressed = false;
			break;
		case 'd':
			key_d_is_pressed = false;
			break;
		case 's':
			key_s_is_pressed = false;
			break;
		case 'o':
			boat.acceleration = 0.4;
			boat.rowing_speed = 10.0;
			break;
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
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/phong_lighting.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/phong_lighting.frag");
	
	// set semantics for the shader variables
	glBindFragDataLocation(shader.getProgramIndex(), 0,"colorOut");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");
	
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

	float amb1[] = { 0.0f, 0.1f, 0.3f, 1.0f };
	float diff1[] = { 0.1f, 0.3f, 0.8f, 1.0f };
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
	water_node = ScenegraphNode(1, &water_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&water_node);
	
	// Islands ------------------------------------------
	island1_element.mesh = createCube();
	memcpy(island1_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(island1_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(island1_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(island1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	island1_element.mesh.mat.shininess = shininess;
	island1_element.mesh.mat.texCount = texcount;
	island1_element.translation = { 50.0F, 0.0F, -70.0F }; //Starting position
	island1_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	island1_element.scale = { 10.0f, 0.5f, 10.0f };
	island1_node = ScenegraphNode(0, &island1_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&island1_node);

	island2_element.mesh = createCube();
	memcpy(island2_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(island2_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(island2_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(island2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	island2_element.mesh.mat.shininess = shininess;
	island2_element.mesh.mat.texCount = texcount;
	island2_element.translation = { 30.0F, 0.0F, 0.0F }; //Starting position
	island2_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	island2_element.scale = { 10.0f, 0.5f, 10.0f };
	island2_node = ScenegraphNode(0, &island2_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&island2_node);
	
	island3_element.mesh = createCube();
	memcpy(island3_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(island3_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(island3_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(island3_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	island3_element.mesh.mat.shininess = shininess;
	island3_element.mesh.mat.texCount = texcount;
	island3_element.translation = { 10.0F, 0.0F, -10.0F }; //Starting position
	island3_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	island3_element.scale = { 10.0f, 0.5f, 10.0f };
	island3_node = ScenegraphNode(0, &island3_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&island3_node);
	
	island4_element.mesh = createCube();
	memcpy(island4_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(island4_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(island4_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(island4_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	island4_element.mesh.mat.shininess = shininess;
	island4_element.mesh.mat.texCount = texcount;
	island4_element.translation = { -20.0F, 0.0F, 40.0F }; //Starting position
	island4_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	island4_element.scale = { 10.0f, 0.5f, 10.0f };
	island4_node = ScenegraphNode(0, &island4_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&island4_node);
	
	// Obstacles ----------------------------------------
	shininess = 100.0f;

	ob1_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob1_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(ob1_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(ob1_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(ob1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob1_element.mesh.mat.shininess = shininess;
	ob1_element.mesh.mat.texCount = texcount;
	ob1_element.translation = { 40.0F, 0.0F, -35.0F }; //Starting position
	ob1_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob1_node = ScenegraphNode(2, &ob1_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&ob1_node);

	ob2_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob2_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(ob2_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(ob2_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(ob2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob2_element.mesh.mat.shininess = shininess;
	ob2_element.mesh.mat.texCount = texcount;
	ob2_element.translation = { 50.0F, 0.0F, -30.0F }; //Starting position
	ob2_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob2_node = ScenegraphNode(3, &ob2_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&ob2_node);

	ob3_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob3_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(ob3_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(ob3_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(ob3_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob3_element.mesh.mat.shininess = shininess;
	ob3_element.mesh.mat.texCount = texcount;
	ob3_element.translation = { 0.0F, 0.0F, 20.0F }; //Starting position
	ob3_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob3_node = ScenegraphNode(4, &ob3_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&ob3_node);

	ob4_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob4_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(ob4_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(ob4_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(ob4_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob4_element.mesh.mat.shininess = shininess;
	ob4_element.mesh.mat.texCount = texcount;
	ob4_element.translation = { 10.0F, 0.0F, 20.0F }; //Starting position
	ob4_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob4_node = ScenegraphNode(5, &ob4_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&ob4_node);

	ob5_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob5_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(ob5_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(ob5_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(ob5_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob5_element.mesh.mat.shininess = shininess;
	ob5_element.mesh.mat.texCount = texcount;
	ob5_element.translation = { -20.0F, 0.0F, 55.0F }; //Starting position
	ob5_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob5_node = ScenegraphNode(6, &ob5_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&ob5_node);

	ob6_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob6_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(ob6_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(ob6_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(ob6_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob6_element.mesh.mat.shininess = shininess;
	ob6_element.mesh.mat.texCount = texcount;
	ob6_element.translation = { -20.0F, 0.0F, 65.0F }; //Starting position
	ob6_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob6_node = ScenegraphNode(7, &ob6_element, &shader, NO_TEXTURE);
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
	house1_node = ScenegraphNode(8, &house1_element, &shader, NO_TEXTURE);
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
	house2_node = ScenegraphNode(9, &house2_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&house2_node);


	// debug boat aabb
	debug1_element.mesh = createCube();
	memcpy(debug1_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(debug1_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(debug1_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(debug1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	debug1_element.mesh.mat.shininess = shininess;
	debug1_element.mesh.mat.texCount = texcount;
	debug1_element.translation = { 0.0F, 0.0F, 0.0F }; //Starting position
	debug1_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	debug1_element.scale = { 0.1F, 0.1F, 0.1F};
	debug1_node = ScenegraphNode(10, &debug1_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&debug1_node);

	debug2_element.mesh = createCube();
	memcpy(debug2_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(debug2_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(debug2_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(debug2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	debug2_element.mesh.mat.shininess = shininess;
	debug2_element.mesh.mat.texCount = texcount;
	debug2_element.translation = { 0.0F, 0.0F, 0.0F }; //Starting position
	debug2_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	debug2_element.scale = { 0.1F, 0.1F, 0.1F };
	debug2_node = ScenegraphNode(11, &debug2_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&debug2_node);
}

void initBoat() {
	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	int texcount = 0;
	float shininess = 100.0f;

	// Boat as a whole
	boat_element.translation = initialBoatPos; //Starting position
	boat_element.rotation = initialBoatRot;
	boat_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	boat_node = ScenegraphNode(0, &boat_element, &shader, 0);
	scenegraph.addNode(&boat_node);

	// Boat Body
	boat_part2_element.mesh = createCube();
	memcpy(boat_part2_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(boat_part2_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(boat_part2_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(boat_part2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	boat_part2_element.mesh.mat.shininess = shininess;
	boat_part2_element.mesh.mat.texCount = texcount;
	boat_part2_element.translation = { -0.5F, 0.0F, -1.0F }; //Starting position
	boat_part2_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	boat_part2_element.scale = { 1.0F, 0.5F, 2.0F, 0.0F};
	boat_part2_node = ScenegraphNode(0, &boat_part2_element, &shader, WOOD_TEXTURE);
	boat_node.addNode(&boat_part2_node);

	// Boad Head
	boat_part1_element.mesh = createCube();
	memcpy(boat_part1_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(boat_part1_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(boat_part1_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(boat_part1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	boat_part1_element.mesh.mat.shininess = shininess;
	boat_part1_element.mesh.mat.texCount = texcount;
	boat_part1_element.translation = { -0.5F, 0.0F, 1.0F }; //Starting position
	boat_part1_element.rotation = { 45.0F, 0.0F, 1.0F, 0.0F };
	boat_part1_element.scale = { (float)sqrt(0.5), 0.5F, (float)sqrt(0.5), 0.0F};
	boat_part1_node = ScenegraphNode(0, &boat_part1_element, &shader, WOOD_TEXTURE);
	boat_node.addNode(&boat_part1_node);

	// Left paddle as a whole
	left_paddle_element.translation = { 0.7F, 0.8F, 0.0F }; //Starting position
	left_paddle_element.rotation = { 0.0F, 1.0F, 0.0F, 0.0F };
	left_paddle_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	left_paddle_node = ScenegraphNode(0, &left_paddle_element, &shader, 0);
	boat_node.addNode(&left_paddle_node);

	// Left paddle stick
	left_paddle_part1_element.mesh = createCylinder(1.5f, 0.05f, 20);
	memcpy(left_paddle_part1_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(left_paddle_part1_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(left_paddle_part1_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(left_paddle_part1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	left_paddle_part1_element.mesh.mat.shininess = shininess;
	left_paddle_part1_element.mesh.mat.texCount = texcount;
	left_paddle_part1_element.translation = { 0.0F, -0.5F, 0.0F }; //Starting position
	left_paddle_part1_element.rotation = { 45.0F, 0.0F, 0.0F, 1.0F };
	left_paddle_part1_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	left_paddle_part1_node = ScenegraphNode(0, &left_paddle_part1_element, &shader, WOOD_TEXTURE);
	left_paddle_node.addNode(&left_paddle_part1_node);

	// Left paddle head
	left_paddle_part2_element.mesh = createCylinder(0.1f, 0.3f, 20);
	memcpy(left_paddle_part2_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(left_paddle_part2_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(left_paddle_part2_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(left_paddle_part2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	left_paddle_part2_element.mesh.mat.shininess = shininess;
	left_paddle_part2_element.mesh.mat.texCount = texcount;
	left_paddle_part2_element.translation = { 0.424F, -0.924F, 0.0F }; //Starting position
	left_paddle_part2_element.rotation = { -245.0F, -1.0F, -0.5F, 1.0F };
	left_paddle_part2_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	left_paddle_part2_node = ScenegraphNode(0, &left_paddle_part2_element, &shader, WOOD_TEXTURE);
	left_paddle_node.addNode(&left_paddle_part2_node);

	// Right paddle as a whole
	right_paddle_element.translation = { -0.7F, 0.8F, 0.0F }; //Starting position
	right_paddle_element.rotation = { 0.0F, 1.0F, 0.0F, 0.0F };
	right_paddle_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	right_paddle_node = ScenegraphNode(0, &right_paddle_element, &shader, WOOD_TEXTURE);
	boat_node.addNode(&right_paddle_node);

	// Right paddle stick
	right_paddle_part1_element.mesh = createCylinder(1.5f, 0.05f, 20);
	memcpy(right_paddle_part1_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(right_paddle_part1_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(right_paddle_part1_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(right_paddle_part1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	right_paddle_part1_element.mesh.mat.shininess = shininess;
	right_paddle_part1_element.mesh.mat.texCount = texcount;
	right_paddle_part1_element.translation = { 0.0F, -0.5F, 0.0F }; //Starting position
	right_paddle_part1_element.rotation = { -45.0F, 0.0F, 0.0F, 1.0F };
	right_paddle_part1_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	right_paddle_part1_node = ScenegraphNode(0, &right_paddle_part1_element, &shader, WOOD_TEXTURE);
	right_paddle_node.addNode(&right_paddle_part1_node);

	// Right paddle head
	right_paddle_part2_element.mesh = createCylinder(0.1f, 0.3f, 20);
	memcpy(right_paddle_part2_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(right_paddle_part2_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(right_paddle_part2_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(right_paddle_part2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	right_paddle_part2_element.mesh.mat.shininess = shininess;
	right_paddle_part2_element.mesh.mat.texCount = texcount;
	right_paddle_part2_element.translation = { -0.424F, -0.924F, 0.0F }; //Starting position
	right_paddle_part2_element.rotation = { 245.0F, 1.0F, -0.5F, 1.0F };
	right_paddle_part2_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	right_paddle_part2_node = ScenegraphNode(0, &right_paddle_part2_element, &shader, WOOD_TEXTURE);
	right_paddle_node.addNode(&right_paddle_part2_node);
}

void initCreatures() {
	float amb[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float diff[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float spec[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	int texcount = 0;
	float shininess = 100.0f;

	float amb1[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float diff1[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float spec1[] = { 0.7f, 0.3f, 0.3f, 1.0f };

	// Set monster movement parameters
	monster1.speed = 5;
	monster1.maxSpeed = 20;
	monster2.speed = 5;
	monster2.maxSpeed = 20;
	monster1.rotationDir = 1;
	monster2.rotationDir = -1;

	// Monster as a whole
	monster1_element.translation = { 3.0F, 0.5F, 0.0F }; //Starting position
	monster1_element.rotation = { -90.0F, 0.0F, 1.0F, 0.0F };
	monster1_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster1_node = ScenegraphNode(0, &monster1_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&monster1_node);

	// Upper jaw
	monster1_part1_element.mesh = createCone(1.5f, 0.5f, 20);
	memcpy(monster1_part1_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(monster1_part1_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(monster1_part1_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(monster1_part1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	monster1_part1_element.mesh.mat.shininess = shininess;
	monster1_part1_element.mesh.mat.texCount = texcount;
	monster1_part1_element.translation = { 0.0F, 0.0F, 0.0F }; //Starting position
	monster1_part1_element.rotation = { 80.0F, 0.0F, 0.0F, 1.0F };
	monster1_part1_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster1_part1_node = ScenegraphNode(0, &monster1_part1_element, &shader, NO_TEXTURE);
	monster1_node.addNode(&monster1_part1_node);

	// Lower jaw
	monster1_part2_element.mesh = createCone(1.5f, 0.5f, 20);
	memcpy(monster1_part2_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(monster1_part2_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(monster1_part2_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(monster1_part2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	monster1_part2_element.mesh.mat.shininess = shininess;
	monster1_part2_element.mesh.mat.texCount = texcount;
	monster1_part2_element.translation = { 0.0F, -0.5F, 0.0F }; //Starting position
	monster1_part2_element.rotation = { 100.0F, 0.0F, 0.0F, 1.0F };
	monster1_part2_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster1_part2_node = ScenegraphNode(0, &monster1_part2_element, &shader, NO_TEXTURE);
	monster1_node.addNode(&monster1_part2_node);

	// Head
	monster1_part3_element.mesh = createSphere(0.85f, 20);
	memcpy(monster1_part3_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(monster1_part3_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(monster1_part3_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(monster1_part3_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	monster1_part3_element.mesh.mat.shininess = shininess;
	monster1_part3_element.mesh.mat.texCount = texcount;
	monster1_part3_element.translation = { 0.5F, -0.2F, 0.0F }; //Starting position
	monster1_part3_element.rotation = { 0.0F, 0.0F, 0.0F, 1.0F };
	monster1_part3_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster1_part3_node = ScenegraphNode(0, &monster1_part3_element, &shader, NO_TEXTURE);
	monster1_node.addNode(&monster1_part3_node);

	// Body
	monster1_part4_element.mesh = createCone(10.0f, 0.7f, 20);
	memcpy(monster1_part4_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(monster1_part4_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(monster1_part4_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(monster1_part4_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	monster1_part4_element.mesh.mat.shininess = shininess;
	monster1_part4_element.mesh.mat.texCount = texcount;
	monster1_part4_element.translation = { 0.2F, -0.2F, 0.0F }; //Starting position
	monster1_part4_element.rotation = { -95.0F, 0.0F, 0.0F, 1.0F };
	monster1_part4_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster1_part4_node = ScenegraphNode(0, &monster1_part4_element, &shader, NO_TEXTURE);
	monster1_node.addNode(&monster1_part4_node);

	// Left Eye
	monster1_part5_element.mesh = createSphere(0.2f, 20);
	memcpy(monster1_part5_element.mesh.mat.ambient, amb1, 4 * sizeof(float));
	memcpy(monster1_part5_element.mesh.mat.diffuse, diff1, 4 * sizeof(float));
	memcpy(monster1_part5_element.mesh.mat.specular, spec1, 4 * sizeof(float));
	memcpy(monster1_part5_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	monster1_part5_element.mesh.mat.shininess = shininess;
	monster1_part5_element.mesh.mat.texCount = texcount;
	monster1_part5_element.translation = { 0.2F, 0.35F, 0.3F }; //Starting position
	monster1_part5_element.rotation = { 0.0F, 0.0F, 0.0F, 1.0F };
	monster1_part5_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster1_part5_node = ScenegraphNode(0, &monster1_part5_element, &shader, NO_TEXTURE);
	monster1_node.addNode(&monster1_part5_node);
	
	// Right Eye
	monster1_part6_element.mesh = createSphere(0.2f, 20);
	memcpy(monster1_part6_element.mesh.mat.ambient, amb1, 4 * sizeof(float));
	memcpy(monster1_part6_element.mesh.mat.diffuse, diff1, 4 * sizeof(float));
	memcpy(monster1_part6_element.mesh.mat.specular, spec1, 4 * sizeof(float));
	memcpy(monster1_part6_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	monster1_part6_element.mesh.mat.shininess = shininess;
	monster1_part6_element.mesh.mat.texCount = texcount;
	monster1_part6_element.translation = { 0.2F, 0.35F, -0.3F }; //Starting position
	monster1_part6_element.rotation = { 0.0F, 0.0F, 0.0F, 1.0F };
	monster1_part6_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster1_part6_node = ScenegraphNode(0, &monster1_part6_element, &shader, NO_TEXTURE);
	monster1_node.addNode(&monster1_part6_node);


	// Monster as a whole
	monster2_element.translation = { 5.0F, 0.5F, 30.0F }; //Starting position
	monster2_element.rotation = { -90.0F, 0.0F, 1.0F, 0.0F };
	monster2_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster2_node = ScenegraphNode(0, &monster2_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&monster2_node);

	// Upper jaw
	monster2_part1_element.mesh = createCone(1.5f, 0.5f, 20);
	memcpy(monster2_part1_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(monster2_part1_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(monster2_part1_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(monster2_part1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	monster2_part1_element.mesh.mat.shininess = shininess;
	monster2_part1_element.mesh.mat.texCount = texcount;
	monster2_part1_element.translation = { 0.0F, 0.0F, 0.0F }; //Starting position
	monster2_part1_element.rotation = { 80.0F, 0.0F, 0.0F, 1.0F };
	monster2_part1_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster2_part1_node = ScenegraphNode(0, &monster2_part1_element, &shader, NO_TEXTURE);
	monster2_node.addNode(&monster2_part1_node);

	// Lower jaw
	monster2_part2_element.mesh = createCone(1.5f, 0.5f, 20);
	memcpy(monster2_part2_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(monster2_part2_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(monster2_part2_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(monster2_part2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	monster2_part2_element.mesh.mat.shininess = shininess;
	monster2_part2_element.mesh.mat.texCount = texcount;
	monster2_part2_element.translation = { 0.0F, -0.5F, 0.0F }; //Starting position
	monster2_part2_element.rotation = { 100.0F, 0.0F, 0.0F, 1.0F };
	monster2_part2_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster2_part2_node = ScenegraphNode(0, &monster2_part2_element, &shader, NO_TEXTURE);
	monster2_node.addNode(&monster2_part2_node);

	// Head
	monster2_part3_element.mesh = createSphere(0.85f, 20);
	memcpy(monster2_part3_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(monster2_part3_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(monster2_part3_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(monster2_part3_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	monster2_part3_element.mesh.mat.shininess = shininess;
	monster2_part3_element.mesh.mat.texCount = texcount;
	monster2_part3_element.translation = { 0.5F, -0.2F, 0.0F }; //Starting position
	monster2_part3_element.rotation = { 0.0F, 0.0F, 0.0F, 1.0F };
	monster2_part3_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster2_part3_node = ScenegraphNode(0, &monster2_part3_element, &shader, NO_TEXTURE);
	monster2_node.addNode(&monster2_part3_node);

	// Body
	monster2_part4_element.mesh = createCone(10.0f, 0.7f, 20);
	memcpy(monster2_part4_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(monster2_part4_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(monster2_part4_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(monster2_part4_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	monster2_part4_element.mesh.mat.shininess = shininess;
	monster2_part4_element.mesh.mat.texCount = texcount;
	monster2_part4_element.translation = { 0.2F, -0.2F, 0.0F }; //Starting position
	monster2_part4_element.rotation = { -95.0F, 0.0F, 0.0F, 1.0F };
	monster2_part4_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster2_part4_node = ScenegraphNode(0, &monster2_part4_element, &shader, NO_TEXTURE);
	monster2_node.addNode(&monster2_part4_node);

	// Left Eye
	monster2_part5_element.mesh = createSphere(0.2f, 20);
	memcpy(monster2_part5_element.mesh.mat.ambient, amb1, 4 * sizeof(float));
	memcpy(monster2_part5_element.mesh.mat.diffuse, diff1, 4 * sizeof(float));
	memcpy(monster2_part5_element.mesh.mat.specular, spec1, 4 * sizeof(float));
	memcpy(monster2_part5_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	monster2_part5_element.mesh.mat.shininess = shininess;
	monster2_part5_element.mesh.mat.texCount = texcount;
	monster2_part5_element.translation = { 0.2F, 0.35F, 0.3F }; //Starting position
	monster2_part5_element.rotation = { 0.0F, 0.0F, 0.0F, 1.0F };
	monster2_part5_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster2_part5_node = ScenegraphNode(0, &monster2_part5_element, &shader, NO_TEXTURE);
	monster2_node.addNode(&monster2_part5_node);

	// Right Eye
	monster2_part6_element.mesh = createSphere(0.2f, 20);
	memcpy(monster2_part6_element.mesh.mat.ambient, amb1, 4 * sizeof(float));
	memcpy(monster2_part6_element.mesh.mat.diffuse, diff1, 4 * sizeof(float));
	memcpy(monster2_part6_element.mesh.mat.specular, spec1, 4 * sizeof(float));
	memcpy(monster2_part6_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	monster2_part6_element.mesh.mat.shininess = shininess;
	monster2_part6_element.mesh.mat.texCount = texcount;
	monster2_part6_element.translation = { 0.2F, 0.35F, -0.3F }; //Starting position
	monster2_part6_element.rotation = { 0.0F, 0.0F, 0.0F, 1.0F };
	monster2_part6_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster2_part6_node = ScenegraphNode(0, &monster2_part6_element, &shader, NO_TEXTURE);
	monster2_node.addNode(&monster2_part6_node);

	respawn_monster(&monster1_node, &monster1);
	respawn_monster(&monster2_node, &monster2);
	monster1.setDirection(monster1_element.rotation);
	monster2.setDirection(monster2_element.rotation);
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

	glGenTextures(3, TextureArray);
	Texture2D_Loader(TextureArray, "stone.tga", 0);
	Texture2D_Loader(TextureArray, "checker.png", 1);
	Texture2D_Loader(TextureArray, "lightwood.tga", 2);

	/// Initialization of freetype library with font_name file
	freeType_init(font_name);

	cameras[0] = Camera(0, 10.0F, 180.0F, 20.0F);
	cameras[1] = Camera(0, 20.0F, 0.0F, 90.0F);
	cameras[2] = Camera(1, 20.0F, 0.0F, 90.0F);

	initBoat();
	initMap();
	initCreatures();

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
	glutKeyboardUpFunc(processUpKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutMouseWheelFunc ( mouseWheel ) ;
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	

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



