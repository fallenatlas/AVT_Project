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
#include <cmath>

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>


// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

#include <IL/il.h>

// Assimp include files
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/scene.h"


// Use Very Simple Libs
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "VertexAttrDef.h"
#include "camera.h"
#include "Scenegraph.hpp"
#include "boat.h"
#include "aabb.h"
#include "Texture_Loader.h"
#include "meshFromAssimp.h"
#include "flare.h"

#include "avtFreeType.h"

using namespace std;

#define CAPTION "AVT Demo: Phong Shading and Text rendered with FreeType"
#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2

#define frand()			((float)rand()/RAND_MAX)
#define M_PI			3.14159265
#define MAX_PARTICULAS  100

int WindowHandle = 0;
int WinX = 1024, WinY = 768;
int currW, currH;

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

//GLint tex_loc, tex_loc1, tex_loc2, tex_loc3, tex_loc4;
//GLuint TextureArray[5];

// Cameras
int activeCamera = 0;
Camera cameras[3];
float ratio = (1.0f * WinX) / WinY;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Frame counting and FPS computation
long myTime, timebase = 0, frame = 0;
char s[32];

// Lights
//float lightPos[4] = {4.0f, 6.0f, 2.0f, 1.0f};
float directionalLightPos[4]{ 10.0f, 30.0f, 10.0f, 0.0f };
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

bool fogActive = true;
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
const int GRASS_TEXTURE = 2;
const int STONE_TEXTURE = 3;
const int PEBBLES_AND_GRASS_TEXTURE = 4;
const int WATER_TEXTURE = 5;
const int LIGHTHOUSE_BILLBOARD_TEXTURE = 6;

// Scene Nodes
ScenegraphNode ground_node;
ScenegraphNode water_node;
ScenegraphNode ob1_node;
ScenegraphNode ob2_node;
ScenegraphNode ob3_node;
ScenegraphNode ob4_node;
ScenegraphNode ob5_node;
ScenegraphNode ob6_node;
ScenegraphNode house1_node;
ScenegraphNode house2_node;
ScenegraphNode house3_node;
ScenegraphNode house4_node;
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
ScenegraphNode monster1_aabb_box_node;
ScenegraphNode monster1_part1_node;
ScenegraphNode monster1_part2_node;
ScenegraphNode monster1_part3_node;
ScenegraphNode monster1_part4_node;
ScenegraphNode monster1_part5_node;
ScenegraphNode monster1_part6_node;
ScenegraphNode monster2_node;
ScenegraphNode monster2_aabb_box_node;
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
ScenegraphNode island1_ground_node;
ScenegraphNode island2_ground_node;
ScenegraphNode island3_ground_node;
ScenegraphNode island4_ground_node;
ScenegraphNode tree1_down_node;
ScenegraphNode tree1_up_node;
ScenegraphNode tree2_down_node;
ScenegraphNode tree2_up_node;
ScenegraphNode tree3_down_node;
ScenegraphNode tree3_up_node;
ScenegraphNode tree4_down_node;
ScenegraphNode tree4_up_node;
ScenegraphNode lighthouse_node;

ScenegraphNode minicooper_node;
ScenegraphNode spider_node;
ScenegraphNode mirror_node;

ScenegraphNode debug1_node;
ScenegraphNode debug2_node;

// Scene Elements
SceneElement ground_element;
SceneElement water_element;
SceneElement ob1_element;
SceneElement ob2_element;
SceneElement ob3_element;
SceneElement ob4_element;
SceneElement ob5_element;
SceneElement ob6_element;
SceneElement house1_element;
SceneElement house2_element;
SceneElement house3_element;
SceneElement house4_element;
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
SceneElement monster1_aabb_box_element;
SceneElement monster1_part1_element;
SceneElement monster1_part2_element;
SceneElement monster1_part3_element;
SceneElement monster1_part4_element;
SceneElement monster1_part5_element;
SceneElement monster1_part6_element;
SceneElement monster2_element;
SceneElement monster2_aabb_box_element;
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
SceneElement island1_ground_element;
SceneElement island2_ground_element;
SceneElement island3_ground_element;
SceneElement island4_ground_element;
SceneElement tree1_down_element;
SceneElement tree1_up_element;
SceneElement tree2_down_element;
SceneElement tree2_up_element;
SceneElement tree3_down_element;
SceneElement tree3_up_element;
SceneElement tree4_down_element;
SceneElement tree4_up_element;

SceneElement lighthouse_element;

SceneElement debug1_element;
SceneElement debug2_element;

SceneElement minicooper_element;
SceneElement spider_element;
SceneElement particle_element;
SceneElement flare_element;
SceneElement mirror_element;

// AABBs
AABB boat_aabb;

//bool update_static_aabbs = true;
AABB island1_aabb;
AABB island2_aabb;
AABB island3_aabb;
AABB island4_aabb;

AABB ob1_aabb;
AABB ob2_aabb;
AABB ob3_aabb;
AABB ob4_aabb;
AABB ob5_aabb;
AABB ob6_aabb;

AABB monster1_aabb;
AABB monster2_aabb;

const std::vector<float> initialBoatPos = { 65.0F, 0.0F, -70.0F };
const std::vector<float> initialBoatRot = { 0.0F, 0.0F, 1.0F, 0.0F };

typedef struct {
	float	life;		// vida
	float	fade;		// fade
	float	r, g, b;    // color
	GLfloat x, y, z;    // posi‹o
	GLfloat vx, vy, vz; // velocidade 
	GLfloat ax, ay, az; // acelera‹o
} Particle;

Particle particula[MAX_PARTICULAS];
int dead_num_particles = MAX_PARTICULAS;

//Flare effect
FLARE_DEF AVTflare;
float lightScreenPos[3];  //Position of the light in Window Coordinates
bool flareEffect = true;
GLuint FlareTextureArray[5];
inline double clamp(const double x, const double min, const double max) {
	return (x < min ? min : (x > max ? max : x));
}

inline int clampi(const int x, const int min, const int max) {
	return (x < min ? min : (x > max ? max : x));
}

// Game variables
float playTime = 0.0F;
int playTimeMinutes = 0;
int playTimeSeconds = 0;
int level = 0;
int livesRemaining = 1;
bool deathOn = false;
bool pauseOn = false;

float monsterBaseSpeed = 5;

// Assimp stuff
// May be changed to another location like the scene node
// Create an instance of the Importer class
Assimp::Importer importer;
Assimp::Importer importer1;

std::string model_dir;  //initialized by the user input at the console
std::string filepath = "backpack/backpack.obj";

// Mirror mesh
MyMesh mirrorMesh;

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
			livesRemaining -= 1;
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

		other_element.translation = { other_element.translation[0] + pushDirection[0] * info.penetration * abs(boat.speed),
									  other_element.translation[1],
									  other_element.translation[2] + pushDirection[2] * info.penetration * abs(boat.speed) };
		boat.speed = 0; // we might potentially need to change this to like velocity
	}
}

void collide_and_resolve_monster(AABB& monster_aabb, AABB& other_aabb) {
	auto info = monster_aabb.intersepts(other_aabb);
	// respawn monster
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
	AABB::getGlobalCubePoints(island1_ground_element.mesh.transform, points1);
	island1_aabb.updateWithVec(points1);

	points1.clear();
	AABB::getGlobalCubePoints(island2_ground_element.mesh.transform, points1);
	island2_aabb.updateWithVec(points1);

	points1.clear();
	AABB::getGlobalCubePoints(island3_ground_element.mesh.transform, points1);
	island3_aabb.updateWithVec(points1);

	points1.clear();
	AABB::getGlobalCubePoints(island4_ground_element.mesh.transform, points1);
	island4_aabb.updateWithVec(points1);

	// update cylinder aabbs
	points1.clear();
	AABB::getGlobalCilinderPoints(ob1_element.mesh.transform, points1);
	ob1_aabb.updateWithVec(points1);

	points1.clear();
	AABB::getGlobalCilinderPoints(ob2_element.mesh.transform, points1);
	ob2_aabb.updateWithVec(points1);

	points1.clear();
	AABB::getGlobalCilinderPoints(ob3_element.mesh.transform, points1);
	ob3_aabb.updateWithVec(points1);

	points1.clear();
	AABB::getGlobalCilinderPoints(ob4_element.mesh.transform, points1);
	ob4_aabb.updateWithVec(points1);

	points1.clear();
	AABB::getGlobalCilinderPoints(ob5_element.mesh.transform, points1);
	ob5_aabb.updateWithVec(points1);

	points1.clear();
	AABB::getGlobalCilinderPoints(ob6_element.mesh.transform, points1);
	ob6_aabb.updateWithVec(points1);

	// update monters aabbs
	// make invis cube as child of main monster node
	points1.clear();
	AABB::getGlobalCubePoints(monster1_aabb_box_element.mesh.transform, points1);
	monster1_aabb.updateWithVec(points1);

	points1.clear();
	AABB::getGlobalCubePoints(monster2_aabb_box_element.mesh.transform, points1);
	monster2_aabb.updateWithVec(points1);

	debug1_element.translation = monster1_aabb.max;
	debug2_element.translation = monster1_aabb.min;

	// test collision with test cube
	collide_and_resolve(boat_aabb, island1_aabb, island1_element, false);
	collide_and_resolve(boat_aabb, island2_aabb, island2_element, false);
	collide_and_resolve(boat_aabb, island3_aabb, island3_element, false);
	collide_and_resolve(boat_aabb, island4_aabb, island4_element, false);

	collide_and_resolve(boat_aabb, ob1_aabb, ob1_element, false);
	collide_and_resolve(boat_aabb, ob2_aabb, ob2_element, false);
	collide_and_resolve(boat_aabb, ob3_aabb, ob3_element, false);
	collide_and_resolve(boat_aabb, ob4_aabb, ob4_element, false);
	collide_and_resolve(boat_aabb, ob5_aabb, ob3_element, false);
	collide_and_resolve(boat_aabb, ob6_aabb, ob4_element, false);

	// test collision boat and monster
	collide_and_resolve(boat_aabb, monster1_aabb, monster1_element, true);
	collide_and_resolve(boat_aabb, monster2_aabb, monster2_element, true);

	// test collision monster and other objs
	//collide_and_resolve_monster(AABB & monster_aabb, AABB & other_aabb)
}

bool paddle_is_in_the_water(SceneElement paddle) {
	return (paddle.rotation[0] >= 0 && ((int)paddle.rotation[0] % 360 < 55 || (int)paddle.rotation[0] % 360 > 310)) ||
		paddle.rotation[0] < 0 && ((int)paddle.rotation[0] % 360 > -50 || (int)paddle.rotation[0] % 360 < -305);
}

void rotateBillboard(SceneElement element, float* cam, float* worldPos) {

	float lookAt[3] = { 0,0,1 }, objToCamProj[3], upAux[3], angleCosine;

	// objToCamProj is the vector in world coordinates from the local origin to the camera
	// projected in the XZ plane
	objToCamProj[0] = cam[0] - worldPos[0];
	objToCamProj[1] = 0;
	objToCamProj[2] = cam[2] - worldPos[2];


	// normalize both vectors to get the cosine directly afterwards
	normalize(objToCamProj);

	// easy fix to determine wether the angle is negative or positive
	// for positive angles upAux will be a vector pointing in the 
	// positive y direction, otherwise upAux will point downwards
	// effectively reversing the rotation.

	crossProduct(lookAt, objToCamProj, upAux);

	// compute the angle
	angleCosine = dotProduct(lookAt, objToCamProj);

	// perform the rotation. The if statement is used for stability reasons
	// if the lookAt and v vectors are too close together then |aux| could
	// be bigger than 1 due to lack of precision
	if ((angleCosine < 0.99990) && (angleCosine > -0.9999)) {
		element.rotation = { (float)(acos(angleCosine) * 180 / 3.14), upAux[0], upAux[1], upAux[2] };
	}
}

// Helper function to multiply a 3D vector by a rotation matrix
void rotateVector(float vec[3], float rotMatrix[3][3], float result[3]) {
	result[0] = rotMatrix[0][0] * vec[0] + rotMatrix[0][1] * vec[1] + rotMatrix[0][2] * vec[2];
	result[1] = rotMatrix[1][0] * vec[0] + rotMatrix[1][1] * vec[1] + rotMatrix[1][2] * vec[2];
	result[2] = rotMatrix[2][0] * vec[0] + rotMatrix[2][1] * vec[1] + rotMatrix[2][2] * vec[2];
}

// Helper function to create a rotation matrix from angle-axis representation
void createRotationMatrixFromAxisAngle(float angle, float x, float y, float z, float rotMatrix[3][3]) {
	float radians = angle * M_PI / 180.0f;  // Convert angle to radians
	float c = cos(radians);
	float s = sin(radians);
	float t = 1.0f - c;

	// Normalize the axis vector (x, y, z)
	float magnitude = sqrt(x * x + y * y + z * z);
	if (magnitude > 0.0f) {
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
	}

	// Compute the rotation matrix based on the axis-angle formula
	rotMatrix[0][0] = t * x * x + c;
	rotMatrix[0][1] = t * x * y - s * z;
	rotMatrix[0][2] = t * x * z + s * y;

	rotMatrix[1][0] = t * x * y + s * z;
	rotMatrix[1][1] = t * y * y + c;
	rotMatrix[1][2] = t * y * z - s * x;

	rotMatrix[2][0] = t * x * z - s * y;
	rotMatrix[2][1] = t * y * z + s * x;
	rotMatrix[2][2] = t * z * z + c;
}

enum PaddleSide {
	LEFT,
	RIGHT
};

void initParticles(PaddleSide side) {
	GLfloat v, theta, phi;
	int i, startIdx, endIdx;
	dead_num_particles = 0;

	// Boat's rotation stored as {angle, x, y, z}
	float angle = boat_element.rotation[0];  // The rotation angle (in degrees)
	float axisX = boat_element.rotation[1];  // The rotation axis (x-component)
	float axisY = boat_element.rotation[2];  // The rotation axis (y-component)
	float axisZ = boat_element.rotation[3];  // The rotation axis (z-component)

	// Create rotation matrix based on boat's axis-angle rotation
	float rotMatrix[3][3];
	createRotationMatrixFromAxisAngle(angle, axisX, axisY, axisZ, rotMatrix);

	// Boat's world position
	float boatPos[3] = { boat_element.translation[0], boat_element.translation[1], boat_element.translation[2] };

	// Set up the range of particles and paddle based on the side
	float paddleLocalPos[3];
	if (side == LEFT) {
		startIdx = 0;
		endIdx = MAX_PARTICULAS / 2;
		paddleLocalPos[0] = left_paddle_element.translation[0] + left_paddle_part2_element.translation[0];
		paddleLocalPos[1] = left_paddle_element.translation[1] + left_paddle_part2_element.translation[1];
		paddleLocalPos[2] = left_paddle_element.translation[2] + left_paddle_part2_element.translation[2];
	}
	else {
		startIdx = MAX_PARTICULAS / 2;
		endIdx = MAX_PARTICULAS;
		paddleLocalPos[0] = right_paddle_element.translation[0] + right_paddle_part2_element.translation[0];
		paddleLocalPos[1] = right_paddle_element.translation[1] + right_paddle_part2_element.translation[1];
		paddleLocalPos[2] = right_paddle_element.translation[2] + right_paddle_part2_element.translation[2];
	}

	for (i = startIdx; i < endIdx; i++) {
		v = 0.8f * frand() + 0.2f;  // Random speed
		phi = frand() * M_PI;       // Random angle for spherical coordinates
		theta = 2.0f * frand() * M_PI;

		// Transform paddle local position using the boat's rotation matrix
		float paddleWorldPos[3];
		rotateVector(paddleLocalPos, rotMatrix, paddleWorldPos);

		// Add boat's world position to get the final particle position
		particula[i].x = boatPos[0] + paddleWorldPos[0];
		particula[i].y = boatPos[1] + paddleWorldPos[1];
		particula[i].z = boatPos[2] + paddleWorldPos[2];

		// Set particle velocity using spherical coordinates
		particula[i].vx = v * cos(theta) * sin(phi);
		particula[i].vy = v * cos(phi);
		particula[i].vz = v * sin(theta) * sin(phi);

		// Apply some acceleration for wind and gravity
		particula[i].ax = 0.1f;  // Simulate wind
		particula[i].ay = 0.15f; // Simulate gravity
		particula[i].az = 0.0f;

		// Set particle color (with a bluish tone)
		particula[i].r = 0.0f;
		particula[i].g = 0.1f;
		particula[i].b = 0.3f;

		// Set initial life and fade
		particula[i].life = 0.05f;    // Initial life
		particula[i].fade = 0.0025f;  // Life decrement per frame
	}
}


void updateParticles()
{
	int i;
	float h;

	/* Método de Euler de integração de eq. diferenciais ordinárias
	h representa o step de tempo; dv/dt = a; dx/dt = v; e conhecem-se os valores iniciais de x e v */

	//h = 0.125f;
	h = 0.033;
	for (i = 0; i < MAX_PARTICULAS; i++)
	{
		particula[i].x += (h * particula[i].vx);
		particula[i].y += (h * particula[i].vy);
		particula[i].z += (h * particula[i].vz);
		particula[i].vx += (h * particula[i].ax);
		particula[i].vy += (h * particula[i].ay);
		particula[i].vz += (h * particula[i].az);
		particula[i].life -= particula[i].fade;
	}
}

void drawParticles() {
	if (paddle_is_in_the_water(left_paddle_element))
		initParticles(LEFT);
	if (paddle_is_in_the_water(right_paddle_element))
		initParticles(RIGHT);

	updateParticles();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthMask(GL_FALSE);  //Depth Buffer Read Only
	glDisable(GL_CULL_FACE);

	float pos[3], right[3], up[3];
	float particle_color[4];

	GLint loc;
	GLint pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	GLint vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	GLint normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	GLint lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
	//GLint texMode_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "texMode"); // different modes of texturing
	GLint normalMap_loc = glGetUniformLocation(shader.getProgramIndex(), "normalMap");
	GLint specularMap_loc = glGetUniformLocation(shader.getProgramIndex(), "specularMap");
	GLint diffMapCount_loc = glGetUniformLocation(shader.getProgramIndex(), "diffMapCount");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, particle_element.textureIds[0]);

	for (int i = 0; i < MAX_PARTICULAS; i++)
	{
		if (particula[i].life > 0.0f) /* só desenha as que ainda estão vivas */
		{
			/* A vida da partícula representa o canal alpha da cor. Como o blend está activo a cor final é a soma da cor rgb do fragmento multiplicada pelo
			alpha com a cor do pixel destino */

			particle_color[0] = particula[i].r;
			particle_color[1] = particula[i].g;
			particle_color[2] = particula[i].b;
			particle_color[3] = particula[i].life;

			// send the material - diffuse color modulated with texture
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, particle_color);
			loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitDiff");
			glUniform1i(loc, 0);
			glUniform1i(normalMap_loc, false);   //GLSL normalMap variable initialized to 0
			glUniform1i(specularMap_loc, false); //
			glUniform1ui(diffMapCount_loc, 1);

			pushMatrix(MODEL);
			translate(MODEL, particula[i].x, particula[i].y, particula[i].z);

			// send matrices to OGL
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			glBindVertexArray(particle_element.mesh.vao);
			glDrawElements(particle_element.mesh.type, particle_element.mesh.numIndexes, GL_UNSIGNED_INT, 0);
			popMatrix(MODEL);
		}
		else dead_num_particles++;
	}

	glDepthMask(GL_TRUE); //make depth buffer again writeable
	glEnable(GL_CULL_FACE);
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
		if (abs(boat.speed) <= 0.2)
			boat.speed = 0;
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
	vector<float> respawn_rotation = { 0.0, 0.0, 1.0, 0.0 };

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
		level += 1;
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
	if (!pauseOn && !deathOn) {
		handle_collisions();

		haddle_movement();

		haddle_monster_movement();

		cameras[0].target[0] = boat_element.translation[0];
		cameras[0].target[1] = boat_element.translation[1];
		cameras[0].target[2] = boat_element.translation[2];

		minicooper_element.translation = boat_element.translation;
		minicooper_element.translation[1] += 1.0f;

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

		playTime += 1.0F / 60.0F;
		playTimeMinutes = (int)playTime / 60;
		playTimeSeconds = (int)playTime % 60;
	}

	if (livesRemaining <= 0) {
		deathOn = true;
	}

	// boat.pos += boat.speed * boat.direction * deltaTime
	// if boat.speed > 0:
	//    boat.speed -= decay

	glutPostRedisplay();
	glutTimerFunc(1000 / 60, refresh, 0);
}

// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	if (h == 0)
		h = 1;
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// set the projection matrix
	// glGetInteger(GL_VIEWPORT, m_view)
	// float ratio = (m_view[2] - m_view[0))/(m_view[3] - m_view[1])

	currW = w;
	currH = h;

	// set the projection matrix
	ratio = (1.0f * w) / h;
	cameras[activeCamera].updateProjectionMatrix(ratio);


}

// ------------------------------------------------------------
//
// Render stufff
//

void setNormalLights(void) {
	//send the light position in eye coordinates
		//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 
	GLint loc;
	float res[4];
	//multMatrixPoint(VIEW, lightPos,res);   //lightPos definido em World Coord so is converted to eye space
	//glUniform4fv(lPos_uniformId, 1, res);

	loc = glGetUniformLocation(shader.getProgramIndex(), "fogOn");
	glUniform1i(loc, fogActive ? 1 : 0);

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
}

void setReflectionLights(void) {
	GLint loc;
	float res[4];
	//multMatrixPoint(VIEW, lightPos,res);   //lightPos definido em World Coord so is converted to eye space
	//glUniform4fv(lPos_uniformId, 1, res);

	loc = glGetUniformLocation(shader.getProgramIndex(), "fogOn");
	glUniform1i(loc, fogActive ? 1 : 0);

	directionalLightDir[0] *= -1.0F;
	loc = glGetUniformLocation(shader.getProgramIndex(), "dirLightOn");
	glUniform1i(loc, dirLightActive ? 1 : 0);
	multMatrixPoint(VIEW, directionalLightDir, res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "dirLight.direction");
	glUniform4fv(loc, 1, res);

	pointLightPos[0][0] *= -1.0F;
	loc = glGetUniformLocation(shader.getProgramIndex(), "pointLightsOn");
	glUniform1i(loc, pointLightsActive ? 1 : 0);
	multMatrixPoint(VIEW, pointLightPos[0], res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "pointLights[0].position");
	glUniform4fv(loc, 1, res);

	pointLightPos[1][0] *= -1.0F;
	multMatrixPoint(VIEW, pointLightPos[1], res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "pointLights[1].position");
	glUniform4fv(loc, 1, res);

	pointLightPos[2][0] *= -1.0F;
	multMatrixPoint(VIEW, pointLightPos[2], res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "pointLights[2].position");
	glUniform4fv(loc, 1, res);

	pointLightPos[3][0] *= -1.0F;
	multMatrixPoint(VIEW, pointLightPos[3], res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "pointLights[3].position");
	glUniform4fv(loc, 1, res);

	pointLightPos[4][0] *= -1.0F;
	multMatrixPoint(VIEW, pointLightPos[4], res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "pointLights[4].position");
	glUniform4fv(loc, 1, res);

	pointLightPos[5][0] *= -1.0F;
	multMatrixPoint(VIEW, pointLightPos[5], res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "pointLights[5].position");
	glUniform4fv(loc, 1, res);

	spotLightPos[0][0] *= -1.0F;
	spotLightDir[0][0] *= -1.0F;
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

	spotLightPos[1][0] *= -1.0F;
	spotLightDir[1][0] *= -1.0F;
	multMatrixPoint(VIEW, spotLightPos[1], res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "spotLights[1].position");
	glUniform4fv(loc, 1, res);
	multMatrixPoint(VIEW, spotLightDir[0], res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "spotLights[1].direction");
	glUniform4fv(loc, 1, res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "spotLights[1].cutOff");
	glUniform1f(loc, cos(12.5f * 3.14f / 180.0f));
}

void drawMirror() {
	// Create the area for the rear view
	/* create a diamond shaped stencil area */
	loadIdentity(PROJECTION);
	if (currW <= currH)
		ortho(-2.0, 2.0, -2.0 * (GLfloat)currH / (GLfloat)currW,
			2.0 * (GLfloat)currH / (GLfloat)currW, -10, 10);
	else
		ortho(-2.0 * (GLfloat)currW / (GLfloat)currH,
			2.0 * (GLfloat)currW / (GLfloat)currW, -2.0, 2.0, -10, 10);

	// load identity matrices for Model-View
	loadIdentity(VIEW);
	loadIdentity(MODEL);

	glUseProgram(shader.getProgramIndex());

	//não vai ser preciso enviar o material pois o cubo não é desenhado

	rotate(MODEL, 0.0f, 0.0, 0.0, 1.0);
	scale(MODEL, 2.0f, 0.6F, 1.0F);
	translate(MODEL, -0.7f, 2.0f, -0.5f);
	// send matrices to OGL
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	//glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	GLint pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	GLint normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	glClear(GL_STENCIL_BUFFER_BIT);

	glStencilFunc(GL_NEVER, 0x3, 0x3);
	glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);

	glBindVertexArray(mirrorMesh.vao);
	glDrawElements(mirrorMesh.type, mirrorMesh.numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	ratio = (1.0f * currW) / currH;
	cameras[0].updateProjectionMatrix(ratio);
}

void renderMirrorView() {

	GLint loc;

	drawMirror();

	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	// set the camera using a function similar to gluLookAt
	vector<float> cameraPos = cameras[activeCamera].getPosition();
	vector<float> direction = { cameraPos[0] - cameras[activeCamera].target[0], cameraPos[1] - cameras[activeCamera].target[1], cameraPos[2] - cameras[activeCamera].target[2] };
	//lookAt(cameraPos[0], cameraPos[1], cameraPos[2], cameraPos[0] + direction[0], cameraPos[1] + direction[1], cameraPos[2] + direction[2], cameras[activeCamera].up[0], cameras[activeCamera].up[1], cameras[activeCamera].up[2]);

	lookAt(boat_element.translation[0], boat_element.translation[1] + 3.0F, boat_element.translation[2], boat_element.translation[0] - boat.direction[0], boat_element.translation[1] + 2.6F, boat_element.translation[2] - boat.direction[2], cameras[activeCamera].up[0], cameras[activeCamera].up[1], cameras[activeCamera].up[2]);

	// use our shader
	//lookAt(cameraPos[0], cameraPos[1], cameraPos[2], cameras[activeCamera].target[0], cameras[activeCamera].target[1], cameras[activeCamera].target[2], cameras[activeCamera].up[0], cameras[activeCamera].up[1], cameras[activeCamera].up[2]);

	glUseProgram(shader.getProgramIndex());

	//send the light position in eye coordinates
	//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

	setNormalLights();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glStencilFunc(GL_EQUAL, 0x3, 0x3);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	ground_node.draw(false, false);
	scenegraph.draw(false, false);
	water_node.draw(false, false);

	cameras[activeCamera].updateProjectionMatrix(ratio);
}

void render_flare(FLARE_DEF* flare, int lx, int ly, int* m_viewport) {  //lx, ly represent the projected position of light on viewport

	int     dx, dy;          // Screen coordinates of "destination"
	int     px, py;          // Screen coordinates of flare element
	int		cx, cy;
	float    maxflaredist, flaredist, flaremaxsize, flarescale, scaleDistance;
	int     width, height, alpha;    // Piece parameters;
	int     i;
	float	diffuse[4];

	GLint loc;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int screenMaxCoordX = m_viewport[0] + m_viewport[2] - 1;
	int screenMaxCoordY = m_viewport[1] + m_viewport[3] - 1;

	//viewport center
	cx = m_viewport[0] + (int)(0.5f * (float)m_viewport[2]) - 1;
	cy = m_viewport[1] + (int)(0.5f * (float)m_viewport[3]) - 1;

	// Compute how far off-center the flare source is.
	maxflaredist = sqrt(cx * cx + cy * cy);
	flaredist = sqrt((lx - cx) * (lx - cx) + (ly - cy) * (ly - cy));
	scaleDistance = (maxflaredist - flaredist) / maxflaredist;
	flaremaxsize = (int)(m_viewport[2] * flare->fMaxSize);
	flarescale = (int)(m_viewport[2] * flare->fScale);

	// Destination is opposite side of centre from source
	dx = clampi(cx + (cx - lx), m_viewport[0], screenMaxCoordX);
	dy = clampi(cy + (cy - ly), m_viewport[1], screenMaxCoordY);

	// Render each element. To be used Texture Unit 0
	GLint pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	GLint vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	GLint normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");

	for (i = 0; i < flare->nPieces; ++i)
	{
		// Position is interpolated along line between start and destination.
		px = (int)((1.0f - flare->element[i].fDistance) * lx + flare->element[i].fDistance * dx);
		py = (int)((1.0f - flare->element[i].fDistance) * ly + flare->element[i].fDistance * dy);
		px = clampi(px, m_viewport[0], screenMaxCoordX);
		py = clampi(py, m_viewport[1], screenMaxCoordY);

		// Piece size are 0 to 1; flare size is proportion of screen width; scale by flaredist/maxflaredist.
		width = (int)(scaleDistance * flarescale * flare->element[i].fSize);

		// Width gets clamped, to allows the off-axis flaresto keep a good size without letting the elements get big when centered.
		if (width > flaremaxsize)  width = flaremaxsize;

		height = (int)((float)m_viewport[3] / (float)m_viewport[2] * (float)width);
		memcpy(diffuse, flare->element[i].matDiffuse, 4 * sizeof(float));
		diffuse[3] *= scaleDistance;   //scale the alpha channel

		if (width > 1)
		{
			// send the material - diffuse color modulated with texture
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, diffuse);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FlareTextureArray[flare->element[i].textureId]);
			pushMatrix(MODEL);
			translate(MODEL, (float)(px - width * 0.0f), (float)(py - height * 0.0f), 0.0f);
			scale(MODEL, (float)width, (float)height, 1);
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			glBindVertexArray(flare_element.mesh.vao);
			glDrawElements(flare_element.mesh.type, flare_element.mesh.numIndexes, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			popMatrix(MODEL);
		}
	}
	glEnable(GL_CULL_FACE);
}

void renderScene(void) {

	GLint loc;

	FrameCount++;
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	renderMirrorView();

	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	// set the camera using a function similar to gluLookAt
	vector<float> cameraPos = cameras[activeCamera].getPosition();
	lookAt(cameraPos[0], cameraPos[1], cameraPos[2], cameras[activeCamera].target[0], cameras[activeCamera].target[1], cameras[activeCamera].target[2], cameras[activeCamera].up[0], cameras[activeCamera].up[1], cameras[activeCamera].up[2]);

	// use our shader
	glUseProgram(shader.getProgramIndex());
	setNormalLights();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// draw scene where the stencil isn't at
	glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// Render the Shadows
	float mat[16];
	GLfloat plano_chao[4] = { 0,1,0,0 };
	GLint shadowMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "shadowMode");

	if (cameraPos[0] > 0.0F) {
		// DO REFLECTIONS
		glStencilFunc(GL_EQUAL, 0x0, 0x3);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

		// Fill stencil buffer with Ground shape; never rendered into color buffer
		glDisable(GL_DEPTH_TEST);
		mirror_node.draw(false, false);
		glEnable(GL_DEPTH_TEST);

		glUniform1i(shadowMode_uniformId, 0);  //iluminação phong

		// Desenhar apenas onde o stencil buffer esta a 1
		glStencilFunc(GL_EQUAL, 0x1, 0x3);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		// Render the reflected geometry
		setReflectionLights();

		glDisable(GL_BLEND);

		pushMatrix(MODEL);
		glCullFace(GL_FRONT);
		scenegraph.draw(false, true);
		popMatrix(MODEL);

		pushMatrix(MODEL);
		scale(MODEL, -1.0f, 1.0f, 1.0f);
		ground_node.draw(false, false);
		popMatrix(MODEL);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		pushMatrix(MODEL);
		scale(MODEL, -1.0f, 1.0f, 1.0f);
		water_node.draw(false, false);
		popMatrix(MODEL);
		glCullFace(GL_BACK);

		setReflectionLights();

		// render mirror
		glUniform1i(shadowMode_uniformId, 0);  //Render with constant color
		mirror_node.draw(false, false);
		// end reflections
	}

	// DO SHADOWS
	glStencilFunc(GL_NOTEQUAL, 0x3, 0x3);
	glStencilOp(GL_KEEP, GL_INCR, GL_INCR);

	// Fill stencil buffer with Ground shape; never rendered into color buffer
	ground_node.draw(false, false);

	// Desenhar apenas onde o stencil buffer esta a 1
	glStencilFunc(GL_EQUAL, 0x1, 0x3);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glUniform1i(shadowMode_uniformId, 0);  //Render with constant color
	ground_node.draw(false, false);

	glUniform1i(shadowMode_uniformId, 1);  //Render with constant color
	glDisable(GL_DEPTH_TEST); //To force the shadow geometry to be rendered even if behind the floor

	//Dark the color stored in color buffer
	glBlendFunc(GL_DST_COLOR, GL_ZERO); //do we want this or not???
	//glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);

	scenegraph.draw(true, false);

	glEnable(GL_DEPTH_TEST);
	// end shadows

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// draw scene where the stencil isn't at
	glStencilFunc(GL_NOTEQUAL, 0x3, 0x3);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	rotateBillboard(lighthouse_element, &cameras[activeCamera].getPosition()[0], &lighthouse_element.translation[0]);

	glUniform1i(shadowMode_uniformId, 0);  //Render with constant color
	scenegraph.draw(false, false);
	water_node.draw(false, false);

	if (boat.speed > 1)
		drawParticles();

	if (flareEffect && dirLightActive) {

		int flarePos[2];
		int m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);

		pushMatrix(MODEL);
		loadIdentity(MODEL);
		computeDerivedMatrix(PROJ_VIEW_MODEL);  //pvm to be applied to lightPost. pvm is used in project function

		if (!project(directionalLightPos, lightScreenPos, m_viewport))
			printf("Error in getting projected light in screen\n");  //Calculate the window Coordinates of the light position: the projected position of light on viewport
		flarePos[0] = clampi((int)lightScreenPos[0], m_viewport[0], m_viewport[0] + m_viewport[2] - 1);
		flarePos[1] = clampi((int)lightScreenPos[1], m_viewport[1], m_viewport[1] + m_viewport[3] - 1);
		popMatrix(MODEL);

		//viewer looking down at  negative z direction
		pushMatrix(PROJECTION);
		loadIdentity(PROJECTION);
		pushMatrix(VIEW);
		loadIdentity(VIEW);
		ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
		render_flare(&AVTflare, flarePos[0], flarePos[1], m_viewport);
		popMatrix(PROJECTION);
		popMatrix(VIEW);
	}

	//Render text (bitmap fonts) in screen coordinates. So use ortoghonal projection with viewport coordinates.
	//the glyph contains transparent background colors and non-transparent for the actual character pixels. So we use the blending

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
	if (deathOn) {
		RenderText(shaderText, "GAME OVER", 370.0f, 384.0f, 1.0f, 0.5f, 0.8f, 0.2f);
	}
	else if (pauseOn) {
		RenderText(shaderText, "PAUSE", 440.0f, 384.0f, 1.0f, 0.5f, 0.8f, 0.2f);
	}
	RenderText(shaderText, "Time: " + to_string(playTimeMinutes) + ":" + to_string(playTimeSeconds), 25.0f, 710.0f, 1.0f, 0.5f, 0.8f, 0.2f);
	RenderText(shaderText, "Lives: " + to_string(livesRemaining), 25.0f, 650.0f, 1.0f, 0.5f, 0.8f, 0.2f);
	RenderText(shaderText, "Level: " + to_string(level), 800.0f, 710.0f, 1.0f, 0.5f, 0.8f, 0.2f);
	//RenderText(shaderText, "This is a sample text", 25.0f, 25.0f, 1.0f, 0.5f, 0.8f, 0.2f);
	//RenderText(shaderText, "AVT Light and Text Rendering Demo", 440.0f, 570.0f, 0.5f, 0.3, 0.7f, 0.9f);
	popMatrix(PROJECTION);
	popMatrix(VIEW);
	popMatrix(MODEL);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glutSwapBuffers();
}

// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy)
{
	switch (key) {

	case 27:
		glutLeaveMainLoop();
		break;
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
	case 'f':
		fogActive = !fogActive;
		break;
	case 'q':
		if (!dirLightActive) flareEffect = false;
		else
			flareEffect = !flareEffect;
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
	case 'p':
		if (!deathOn)
			pauseOn = !pauseOn;
		break;
	case 'r':
		deathOn = false;
		livesRemaining = 3;
		playTime = 0.0;
		level = 0;
		boat_element.translation = initialBoatPos;
		boat_element.rotation = initialBoatRot;
		boat.speed = 0;
		boat.setDirection(boat_element.rotation);
		cameras[0].boatAngle = boat_element.rotation[0];
		cameras[0].setOffset();

		monster1.speed = monsterBaseSpeed;
		monster2.speed = monsterBaseSpeed;
		monster_speed_timer = 0.0F;
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
	if (state == GLUT_DOWN) {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1 && activeCamera == 0) {
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
	if (activeCamera == 2) {
		return;
	}

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX = -xx + startX;
	deltaY = yy - startY;

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
	else {
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
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/phong_lighting_new.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/phong_lighting_new.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shader.getProgramIndex(), 0, "colorOut");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shader.getProgramIndex());
	printf("InfoLog for Model Rendering Shader\n%s\n\n", shaderText.getAllInfoLogs().c_str());

	if (!shader.isProgramValid()) {
		printf("GLSL Model Program Not Valid!\n");
		exit(1);
	}

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

void initRearViewMirror() {
	float amb1[] = { 0.3f, 0.0f, 0.0f, 1.0f };
	float diff1[] = { 0.8f, 0.1f, 0.1f, 1.0f };
	float spec1[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texcount = 0;

	// create geometry and VAO of the cube
	mirrorMesh = createCube();
	memcpy(mirrorMesh.mat.ambient, amb1, 4 * sizeof(float));
	memcpy(mirrorMesh.mat.diffuse, diff1, 4 * sizeof(float));
	memcpy(mirrorMesh.mat.specular, spec1, 4 * sizeof(float));
	memcpy(mirrorMesh.mat.emissive, emissive, 4 * sizeof(float));
	mirrorMesh.mat.shininess = shininess;
	mirrorMesh.mat.texCount = texcount;
}

void initMap()
{
	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	int texcount = 0;

	float amb1[] = { 0.0f, 0.1f, 0.3f, 0.8f };
	float diff1[] = { 0.1f, 0.3f, 0.8f, 0.8f };
	float spec1[] = { 0.9f, 0.9f, 0.9f, 0.8f };
	float shininess = 5000.0;

	// ground -------------------------------------------
	ground_element.mesh = createQuad(320.0F, 320.0F);
	memcpy(ground_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(ground_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(ground_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(ground_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ground_element.mesh.mat.shininess = shininess;
	ground_element.mesh.mat.texCount = texcount;
	ground_element.translation = { 0.0F, -2.0F, 0.0F };
	ground_element.rotation = { -90.0F, 1.0F, 0.0F, 0.0F };
	ground_node = ScenegraphNode(&ground_element, &shader, NO_TEXTURE);
	//scenegraph.addNode(&ground_node);

	// Islands ------------------------------------------
	island1_element.translation = { 50.0F, 0.0F, -70.0F }; //Starting position
	island1_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	island1_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	island1_ground_node = ScenegraphNode(&island1_ground_element, &shader, NO_TEXTURE);
	island1_node = ScenegraphNode(&island1_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&island1_node);

	island1_ground_element.mesh = createCube();
	memcpy(island1_ground_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(island1_ground_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(island1_ground_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(island1_ground_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	island1_ground_element.mesh.mat.shininess = shininess;
	island1_ground_element.mesh.mat.texCount = texcount;
	island1_ground_element.translation = { 0.0F, -2.0F, 0.0F }; //Starting position
	island1_ground_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	island1_ground_element.scale = { 10.0f, 2.5f, 10.0f };
	island1_ground_node = ScenegraphNode(&island1_ground_element, &shader, PEBBLES_AND_GRASS_TEXTURE);
	island1_node.addNode(&island1_ground_node);

	island2_element.translation = { 30.0F, 0.0F, 0.0F }; //Starting position
	island2_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	island2_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	island2_node = ScenegraphNode(&island2_element, &shader, 0);
	scenegraph.addNode(&island2_node);

	island2_ground_element.mesh = createCube();
	memcpy(island2_ground_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(island2_ground_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(island2_ground_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(island2_ground_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	island2_ground_element.mesh.mat.shininess = shininess;
	island2_ground_element.mesh.mat.texCount = texcount;
	island2_ground_element.translation = { 0.0F, -2.0F, 0.0F }; //Starting position
	island2_ground_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	island2_ground_element.scale = { 10.0f, 2.5f, 10.0f };
	island2_ground_node = ScenegraphNode(&island2_ground_element, &shader, PEBBLES_AND_GRASS_TEXTURE);
	island2_node.addNode(&island2_ground_node);

	island3_element.translation = { 10.0F, 0.0F, -10.0F }; //Starting position
	island3_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	island3_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	island3_node = ScenegraphNode(&island3_element, &shader, 0);
	scenegraph.addNode(&island3_node);

	island3_ground_element.mesh = createCube();
	memcpy(island3_ground_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(island3_ground_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(island3_ground_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(island3_ground_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	island3_ground_element.mesh.mat.shininess = shininess;
	island3_ground_element.mesh.mat.texCount = texcount;
	island3_ground_element.translation = { 0.0F, -2.0F, 0.0F }; //Starting position
	island3_ground_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	island3_ground_element.scale = { 10.0f, 2.5f, 10.0f };
	island3_ground_node = ScenegraphNode(&island3_ground_element, &shader, PEBBLES_AND_GRASS_TEXTURE);
	island3_node.addNode(&island3_ground_node);

	island4_element.translation = { -20.0F, 0.0F, 40.0F }; //Starting position
	island4_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	island4_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	island4_node = ScenegraphNode(&island4_element, &shader, 0);
	scenegraph.addNode(&island4_node);

	island4_ground_element.mesh = createCube();
	memcpy(island4_ground_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(island4_ground_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(island4_ground_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(island4_ground_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	island4_ground_element.mesh.mat.shininess = shininess;
	island4_ground_element.mesh.mat.texCount = texcount;
	island4_ground_element.translation = { 0.0F, -2.0F, 0.0F }; //Starting position
	island4_ground_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	island4_ground_element.scale = { 10.0f, 2.5f, 10.0f };
	island4_ground_node = ScenegraphNode(&island4_ground_element, &shader, PEBBLES_AND_GRASS_TEXTURE);
	island4_node.addNode(&island4_ground_node);

	// Houses ------------------------------------------
	house1_element.mesh = createCube();
	memcpy(house1_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(house1_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(house1_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(house1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	house1_element.mesh.mat.shininess = shininess;
	house1_element.mesh.mat.texCount = texcount;
	house1_element.translation = { 5.0F, 0.5F, 5.0F }; //Starting position
	house1_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	house1_node = ScenegraphNode(&house1_element, &shader, STONE_TEXTURE);
	island1_node.addNode(&house1_node);

	house2_element.mesh = createCube();
	memcpy(house2_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(house2_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(house2_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(house2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	house2_element.mesh.mat.shininess = shininess;
	house2_element.mesh.mat.texCount = texcount;
	house2_element.translation = { 5.0F, 0.5F, 5.0F }; //Starting position
	house2_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	house2_node = ScenegraphNode(&house2_element, &shader, STONE_TEXTURE);
	island2_node.addNode(&house2_node);

	house3_element.mesh = createCube();
	memcpy(house3_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(house3_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(house3_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(house3_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	house3_element.mesh.mat.shininess = shininess;
	house3_element.mesh.mat.texCount = texcount;
	house3_element.translation = { 5.0F, 0.5F, 5.0F }; //Starting position
	house3_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	house3_node = ScenegraphNode(&house3_element, &shader, STONE_TEXTURE);
	island3_node.addNode(&house3_node);

	house4_element.mesh = createCube();
	memcpy(house4_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(house4_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(house4_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(house4_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	house4_element.mesh.mat.shininess = shininess;
	house4_element.mesh.mat.texCount = texcount;
	house4_element.translation = { 5.0F, 0.5F, 5.0F }; //Starting position
	house4_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	house4_node = ScenegraphNode(&house4_element, &shader, STONE_TEXTURE);
	island4_node.addNode(&house4_node);


	// Trees --------------------------------------------

	tree1_down_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(tree1_down_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(tree1_down_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(tree1_down_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(tree1_down_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	tree1_down_element.mesh.mat.shininess = shininess;
	tree1_down_element.mesh.mat.texCount = texcount;
	tree1_down_element.translation = { 3.0F, 0.5F, 3.0F }; //Starting position
	tree1_down_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	tree1_down_node = ScenegraphNode(&tree1_down_element, &shader, WOOD_TEXTURE);
	island1_node.addNode(&tree1_down_node);

	tree1_up_element.mesh = createSphere(1.0, 20);
	memcpy(tree1_up_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(tree1_up_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(tree1_up_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(tree1_up_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	tree1_up_element.mesh.mat.shininess = shininess;
	tree1_up_element.mesh.mat.texCount = texcount;
	tree1_up_element.translation = { 3.0F, 2.0F, 3.0F }; //Starting position
	tree1_up_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	tree1_up_node = ScenegraphNode(&tree1_up_element, &shader, GRASS_TEXTURE);
	island1_node.addNode(&tree1_up_node);

	tree2_down_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(tree2_down_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(tree2_down_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(tree2_down_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(tree2_down_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	tree2_down_element.mesh.mat.shininess = shininess;
	tree2_down_element.mesh.mat.texCount = texcount;
	tree2_down_element.translation = { 3.0F, 0.5F, 3.0F }; //Starting position
	tree2_down_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	tree2_down_node = ScenegraphNode(&tree2_down_element, &shader, WOOD_TEXTURE);
	island2_node.addNode(&tree2_down_node);

	tree2_up_element.mesh = createSphere(1.0, 20);
	memcpy(tree2_up_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(tree2_up_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(tree2_up_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(tree2_up_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	tree2_up_element.mesh.mat.shininess = shininess;
	tree2_up_element.mesh.mat.texCount = texcount;
	tree2_up_element.translation = { 3.0F, 2.0F, 3.0F }; //Starting position
	tree2_up_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	tree2_up_node = ScenegraphNode(&tree2_up_element, &shader, GRASS_TEXTURE);
	island2_node.addNode(&tree2_up_node);

	tree3_down_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(tree3_down_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(tree3_down_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(tree3_down_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(tree3_down_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	tree3_down_element.mesh.mat.shininess = shininess;
	tree3_down_element.mesh.mat.texCount = texcount;
	tree3_down_element.translation = { 3.0F, 0.5F, 3.0F }; //Starting position
	tree3_down_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	tree3_down_node = ScenegraphNode(&tree3_down_element, &shader, WOOD_TEXTURE);
	island3_node.addNode(&tree3_down_node);

	tree3_up_element.mesh = createSphere(1.0, 20);
	memcpy(tree3_up_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(tree3_up_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(tree3_up_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(tree3_up_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	tree3_up_element.mesh.mat.shininess = shininess;
	tree3_up_element.mesh.mat.texCount = texcount;
	tree3_up_element.translation = { 3.0F, 2.0F, 3.0F }; //Starting position
	tree3_up_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	tree3_up_node = ScenegraphNode(&tree3_up_element, &shader, GRASS_TEXTURE);
	island3_node.addNode(&tree3_up_node);

	tree4_down_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(tree4_down_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(tree4_down_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(tree4_down_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(tree4_down_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	tree4_down_element.mesh.mat.shininess = shininess;
	tree4_down_element.mesh.mat.texCount = texcount;
	tree4_down_element.translation = { 3.0F, 0.5F, 3.0F }; //Starting position
	tree4_down_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	tree4_down_node = ScenegraphNode(&tree4_down_element, &shader, WOOD_TEXTURE);
	island4_node.addNode(&tree4_down_node);

	tree4_up_element.mesh = createSphere(1.0, 20);
	memcpy(tree4_up_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(tree4_up_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(tree4_up_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(tree4_up_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	tree4_up_element.mesh.mat.shininess = shininess;
	tree4_up_element.mesh.mat.texCount = texcount;
	tree4_up_element.translation = { 3.0F, 2.0F, 3.0F }; //Starting position
	tree4_up_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	tree4_up_node = ScenegraphNode(&tree4_up_element, &shader, GRASS_TEXTURE);
	island4_node.addNode(&tree4_up_node);


	// Obstacles ----------------------------------------
	float amb2[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff2[] = { 1.0f, 0.2f, 0.1f, 1.0f };
	float spec2[] = { 1.0f, 0.8f, 0.8f, 1.0f };
	shininess = 100.0f;

	ob1_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob1_element.mesh.mat.ambient, amb2, 4 * sizeof(float));
	memcpy(ob1_element.mesh.mat.diffuse, diff2, 4 * sizeof(float));
	memcpy(ob1_element.mesh.mat.specular, spec2, 4 * sizeof(float));
	memcpy(ob1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob1_element.mesh.mat.shininess = shininess;
	ob1_element.mesh.mat.texCount = texcount;
	ob1_element.translation = { 40.0F, 0.0F, -35.0F }; //Starting position
	ob1_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob1_node = ScenegraphNode(&ob1_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&ob1_node);

	ob2_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob2_element.mesh.mat.ambient, amb2, 4 * sizeof(float));
	memcpy(ob2_element.mesh.mat.diffuse, diff2, 4 * sizeof(float));
	memcpy(ob2_element.mesh.mat.specular, spec2, 4 * sizeof(float));
	memcpy(ob2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob2_element.mesh.mat.shininess = shininess;
	ob2_element.mesh.mat.texCount = texcount;
	ob2_element.translation = { 50.0F, 0.0F, -30.0F }; //Starting position
	ob2_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob2_node = ScenegraphNode(&ob2_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&ob2_node);

	ob3_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob3_element.mesh.mat.ambient, amb2, 4 * sizeof(float));
	memcpy(ob3_element.mesh.mat.diffuse, diff2, 4 * sizeof(float));
	memcpy(ob3_element.mesh.mat.specular, spec2, 4 * sizeof(float));
	memcpy(ob3_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob3_element.mesh.mat.shininess = shininess;
	ob3_element.mesh.mat.texCount = texcount;
	ob3_element.translation = { 0.0F, 0.0F, 20.0F }; //Starting position
	ob3_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob3_node = ScenegraphNode(&ob3_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&ob3_node);

	ob4_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob4_element.mesh.mat.ambient, amb2, 4 * sizeof(float));
	memcpy(ob4_element.mesh.mat.diffuse, diff2, 4 * sizeof(float));
	memcpy(ob4_element.mesh.mat.specular, spec2, 4 * sizeof(float));
	memcpy(ob4_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob4_element.mesh.mat.shininess = shininess;
	ob4_element.mesh.mat.texCount = texcount;
	ob4_element.translation = { 10.0F, 0.0F, 20.0F }; //Starting position
	ob4_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob4_node = ScenegraphNode(&ob4_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&ob4_node);

	ob5_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob5_element.mesh.mat.ambient, amb2, 4 * sizeof(float));
	memcpy(ob5_element.mesh.mat.diffuse, diff2, 4 * sizeof(float));
	memcpy(ob5_element.mesh.mat.specular, spec2, 4 * sizeof(float));
	memcpy(ob5_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob5_element.mesh.mat.shininess = shininess;
	ob5_element.mesh.mat.texCount = texcount;
	ob5_element.translation = { -20.0F, 0.0F, 55.0F }; //Starting position
	ob5_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob5_node = ScenegraphNode(&ob5_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&ob5_node);

	ob6_element.mesh = createCylinder(2.0f, 0.4f, 20);
	memcpy(ob6_element.mesh.mat.ambient, amb2, 4 * sizeof(float));
	memcpy(ob6_element.mesh.mat.diffuse, diff2, 4 * sizeof(float));
	memcpy(ob6_element.mesh.mat.specular, spec2, 4 * sizeof(float));
	memcpy(ob6_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	ob6_element.mesh.mat.shininess = shininess;
	ob6_element.mesh.mat.texCount = texcount;
	ob6_element.translation = { -20.0F, 0.0F, 65.0F }; //Starting position
	ob6_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	ob6_node = ScenegraphNode(&ob6_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&ob6_node);

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
	debug1_element.scale = { 0.1F, 0.1F, 0.1F };
	debug1_node = ScenegraphNode(&debug1_element, &shader, NO_TEXTURE);
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
	debug2_node = ScenegraphNode(&debug2_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&debug2_node);

	// LightHouse (billboard) -------------------------------------------
	lighthouse_element.mesh = createQuad(40.0F, 40.0F);
	memcpy(lighthouse_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(lighthouse_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(lighthouse_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(lighthouse_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	lighthouse_element.mesh.mat.shininess = shininess;
	lighthouse_element.mesh.mat.texCount = texcount;
	lighthouse_element.translation = { 75.0F, 13.0F, 160.0F };
	lighthouse_element.rotation = { 180.0F, 0.0F, 1.0F, 0.0F };
	lighthouse_node = ScenegraphNode(&lighthouse_element, &shader, LIGHTHOUSE_BILLBOARD_TEXTURE);
	scenegraph.addNode(&lighthouse_node);

	// Water -------------------------------------------
	water_element.mesh = createQuad(320.0F, 320.0F);
	memcpy(water_element.mesh.mat.ambient, amb1, 4 * sizeof(float));
	memcpy(water_element.mesh.mat.diffuse, diff1, 4 * sizeof(float));
	memcpy(water_element.mesh.mat.specular, spec1, 4 * sizeof(float));
	memcpy(water_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	water_element.mesh.mat.shininess = shininess;
	water_element.mesh.mat.texCount = texcount;
	water_element.translation = { 0.0F, 0.0F, 0.0F };
	water_element.rotation = { -90.0F, 1.0F, 0.0F, 0.0F };
	water_node = ScenegraphNode(&water_element, &shader, WATER_TEXTURE);
	//scenegraph.addNode(&water_node);

	// Water particles
	particle_element.mesh = createQuad(1, 1);
	particle_element.mesh.mat.texCount = texcount;

	// Flare
	flare_element.mesh = createQuad(1, 1);
	flare_element.mesh.mat.texCount = texcount;

	//Load flare from file
	loadFlareFile(&AVTflare, "flare.txt");
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
	boat_node = ScenegraphNode(&boat_element, &shader, 0);
	scenegraph.addNode(&boat_node);

	// Boat Body
	boat_part2_element.mesh = createCube();
	memcpy(boat_part2_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(boat_part2_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(boat_part2_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(boat_part2_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	boat_part2_element.mesh.mat.shininess = shininess;
	boat_part2_element.mesh.mat.texCount = texcount;
	boat_part2_element.translation = { -0.5F, -0.2F, -1.0F }; //Starting position
	boat_part2_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
	boat_part2_element.scale = { 1.0F, 0.7F, 2.0F, 0.0F };
	boat_part2_node = ScenegraphNode(&boat_part2_element, &shader, WOOD_TEXTURE);
	boat_node.addNode(&boat_part2_node);

	// Boad Head
	boat_part1_element.mesh = createCube();
	memcpy(boat_part1_element.mesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(boat_part1_element.mesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(boat_part1_element.mesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(boat_part1_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	boat_part1_element.mesh.mat.shininess = shininess;
	boat_part1_element.mesh.mat.texCount = texcount;
	boat_part1_element.translation = { -0.5F, -0.2F, 1.0F }; //Starting position
	boat_part1_element.rotation = { 45.0F, 0.0F, 1.0F, 0.0F };
	boat_part1_element.scale = { (float)sqrt(0.5), 0.7F, (float)sqrt(0.5), 0.0F };
	boat_part1_node = ScenegraphNode(&boat_part1_element, &shader, WOOD_TEXTURE);
	boat_node.addNode(&boat_part1_node);

	// Left paddle as a whole
	left_paddle_element.translation = { 0.7F, 0.8F, 0.0F }; //Starting position
	left_paddle_element.rotation = { 0.0F, 1.0F, 0.0F, 0.0F };
	left_paddle_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	left_paddle_node = ScenegraphNode(&left_paddle_element, &shader, 0);
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
	left_paddle_part1_node = ScenegraphNode(&left_paddle_part1_element, &shader, WOOD_TEXTURE);
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
	left_paddle_part2_node = ScenegraphNode(&left_paddle_part2_element, &shader, WOOD_TEXTURE);
	left_paddle_node.addNode(&left_paddle_part2_node);

	// Right paddle as a whole
	right_paddle_element.translation = { -0.7F, 0.8F, 0.0F }; //Starting position
	right_paddle_element.rotation = { 0.0F, 1.0F, 0.0F, 0.0F };
	right_paddle_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	right_paddle_node = ScenegraphNode(&right_paddle_element, &shader, WOOD_TEXTURE);
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
	right_paddle_part1_node = ScenegraphNode(&right_paddle_part1_element, &shader, WOOD_TEXTURE);
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
	right_paddle_part2_node = ScenegraphNode(&right_paddle_part2_element, &shader, WOOD_TEXTURE);
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
	monster1.speed = monsterBaseSpeed;
	monster1.maxSpeed = 20;
	monster2.speed = monsterBaseSpeed;
	monster2.maxSpeed = 20;
	monster1.rotationDir = 1;
	monster2.rotationDir = -1;

	// Monster as a whole
	monster1_element.translation = { 3.0F, 0.5F, 0.0F }; //Starting position
	monster1_element.rotation = { -90.0F, 0.0F, 1.0F, 0.0F };
	monster1_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster1_node = ScenegraphNode(&monster1_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&monster1_node);

	monster1_aabb_box_element.translation = { -0.75F, -0.75F, -0.85F }; //Starting position
	monster1_aabb_box_element.rotation = { 0.0F, 0.0F, 0.0F, 1.0F };
	monster1_aabb_box_element.scale = { 6.0F, 1.5F, 1.7F, 0.0F };
	monster1_aabb_box_node = ScenegraphNode(&monster1_aabb_box_element, &shader, NO_TEXTURE);
	monster1_node.addNode(&monster1_aabb_box_node);

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
	monster1_part1_node = ScenegraphNode(&monster1_part1_element, &shader, NO_TEXTURE);
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
	monster1_part2_node = ScenegraphNode(&monster1_part2_element, &shader, NO_TEXTURE);
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
	monster1_part3_node = ScenegraphNode(&monster1_part3_element, &shader, NO_TEXTURE);
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
	monster1_part4_node = ScenegraphNode(&monster1_part4_element, &shader, NO_TEXTURE);
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
	monster1_part5_node = ScenegraphNode(&monster1_part5_element, &shader, NO_TEXTURE);
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
	monster1_part6_node = ScenegraphNode(&monster1_part6_element, &shader, NO_TEXTURE);
	monster1_node.addNode(&monster1_part6_node);


	// Monster as a whole
	monster2_element.translation = { 5.0F, 0.5F, 30.0F }; //Starting position
	monster2_element.rotation = { -90.0F, 0.0F, 1.0F, 0.0F };
	monster2_element.scale = { 1.0F, 1.0F, 1.0F, 0.0F };
	monster2_node = ScenegraphNode(&monster2_element, &shader, NO_TEXTURE);
	scenegraph.addNode(&monster2_node);

	monster2_aabb_box_element.translation = { -0.75F, -0.75F, -0.85F }; //Starting position
	monster2_aabb_box_element.rotation = { 0.0F, 0.0F, 0.0F, 1.0F };
	monster2_aabb_box_element.scale = { 6.0F, 1.5F, 1.7F, 0.0F };
	monster2_aabb_box_node = ScenegraphNode(&monster2_aabb_box_element, &shader, NO_TEXTURE);
	monster2_node.addNode(&monster2_aabb_box_node);

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
	monster2_part1_node = ScenegraphNode(&monster2_part1_element, &shader, NO_TEXTURE);
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
	monster2_part2_node = ScenegraphNode(&monster2_part2_element, &shader, NO_TEXTURE);
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
	monster2_part3_node = ScenegraphNode(&monster2_part3_element, &shader, NO_TEXTURE);
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
	monster2_part4_node = ScenegraphNode(&monster2_part4_element, &shader, NO_TEXTURE);
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
	monster2_part5_node = ScenegraphNode(&monster2_part5_element, &shader, NO_TEXTURE);
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
	monster2_part6_node = ScenegraphNode(&monster2_part6_element, &shader, NO_TEXTURE);
	monster2_node.addNode(&monster2_part6_node);

	respawn_monster(&monster1_node, &monster1);
	respawn_monster(&monster2_node, &monster2);
	monster1.setDirection(monster1_element.rotation);
	monster2.setDirection(monster2_element.rotation);
}

void initMirror() {
	float amb[] = { 0.0f, 0.0f, 0.2f, 0.3f };
	float diff[] = { 0.0f, 0.0f, 0.99f, 1.0f };
	float amb2[] = { 0.1f, 0.1f, 0.1f, 0.3f };
	float diff2[] = { 0.99f, 0.99f, 0.99f, 1.0f };
	float diff3[] = { 0.99f, 0.99f, 0.99f, 0.3f };
	float spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float spec2[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texcount = 0;

	mirror_element.mesh = createCube();
	memcpy(mirror_element.mesh.mat.ambient, amb2, 4 * sizeof(float));
	memcpy(mirror_element.mesh.mat.diffuse, diff3, 4 * sizeof(float));
	memcpy(mirror_element.mesh.mat.specular, spec2, 4 * sizeof(float));
	memcpy(mirror_element.mesh.mat.emissive, emissive, 4 * sizeof(float));
	mirror_element.mesh.mat.shininess = shininess;
	mirror_element.mesh.mat.texCount = texcount;
	mirror_element.translation = { 0.0F, 0.0F, -5.0F };
	mirror_element.scale = { 0.005f, 20.0F, 20.0F };
	mirror_node = ScenegraphNode(&mirror_element, &shader, NO_TEXTURE);
}

void load_texture(SceneElement& element, int numTextures, std::vector<std::string> filenames) {
	element.numTextures = numTextures;
	element.textureIds = new GLuint[numTextures];
	glGenTextures(numTextures, element.textureIds); // Texture name generation 

	// load 1 or 2 texture according to the numTextures
	for (int i = 0; i < numTextures; i++) {
		Texture2D_Loader(element.textureIds, filenames[i].c_str(), i);
	}
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

	cameras[0] = Camera(0, 10.0F, 180.0F, 20.0F);
	cameras[1] = Camera(0, 160.0F, 0.0F, 90.0F);
	cameras[2] = Camera(1, 20.0F, 0.0F, 90.0F);

	initRearViewMirror();
	initBoat();
	initCreatures();
	initMap(); // this needs to be last so that water is drawn last (because of transparency)
	initMirror();

	// load textures
	// islands ground
	load_texture(island1_ground_element, 2, { "pebbles.jpg", "grass.tga" });
	load_texture(island2_ground_element, 2, { "pebbles.jpg", "grass.tga" });
	load_texture(island3_ground_element, 2, { "pebbles.jpg", "grass.tga" });
	load_texture(island4_ground_element, 2, { "pebbles.jpg", "grass.tga" });

	// houses
	load_texture(house1_element, 1, { "stone.tga" });
	load_texture(house2_element, 1, { "stone.tga" });
	load_texture(house3_element, 1, { "stone.tga" });
	load_texture(house4_element, 1, { "stone.tga" });

	// trees
	load_texture(tree1_up_element, 1, { "grass.tga" });
	load_texture(tree2_up_element, 1, { "grass.tga" });
	load_texture(tree3_up_element, 1, { "grass.tga" });
	load_texture(tree4_up_element, 1, { "grass.tga" });

	load_texture(tree1_down_element, 1, { "lightwood.tga" });
	load_texture(tree2_down_element, 1, { "lightwood.tga" });
	load_texture(tree3_down_element, 1, { "lightwood.tga" });
	load_texture(tree4_down_element, 1, { "lightwood.tga" });

	// water
	load_texture(water_element, 1, { "water.jpg" });
	load_texture(particle_element, 1, { "water_particle.png" });

	// lighthouse
	load_texture(lighthouse_element, 1, { "lighthouse.png" });

	//boat
	load_texture(boat_part1_element, 1, { "lightwood.tga" });
	load_texture(boat_part2_element, 1, { "lightwood.tga" });
	load_texture(left_paddle_part1_element, 1, { "lightwood.tga" });
	load_texture(left_paddle_part2_element, 1, { "lightwood.tga" });
	load_texture(right_paddle_part1_element, 1, { "lightwood.tga" });
	load_texture(right_paddle_part2_element, 1, { "lightwood.tga" });

	//Flare elements textures
	glGenTextures(5, FlareTextureArray);
	Texture2D_Loader(FlareTextureArray, "crcl.tga", 0);
	Texture2D_Loader(FlareTextureArray, "flar.tga", 1);
	Texture2D_Loader(FlareTextureArray, "hxgn.tga", 2);
	Texture2D_Loader(FlareTextureArray, "ring.tga", 3);
	Texture2D_Loader(FlareTextureArray, "sun.tga", 4);

	float scaleFactor;

	// Test assimp mesh
	//import 3D file into Assimp scene graph
	model_dir = "backpack/";
	bool res = Import3DFromFile(filepath, importer, minicooper_element.scene, scaleFactor);

	if (res) {
		//creation of Mymesh array with VAO Geometry and Material and array of Texture Objs for the model input by the user
		minicooper_element.meshes = createMeshFromAssimp(minicooper_element.scene, minicooper_element.textureIds);

		minicooper_element.translation = { 30.0F, 5.0F, 10.0F }; //Starting position
		minicooper_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
		minicooper_element.scale = { scaleFactor, scaleFactor, scaleFactor };
		minicooper_element.usingAssimp = true;
		minicooper_element.normalMapKey = true;
		minicooper_node = ScenegraphNode(&minicooper_element, &shader, 0);
		scenegraph.addNode(&minicooper_node);
	}

	model_dir = "spider/";
	res = Import3DFromFile("spider/spider.obj", importer1, spider_element.scene, scaleFactor);

	if (res) {
		//creation of Mymesh array with VAO Geometry and Material and array of Texture Objs for the model input by the user
		spider_element.meshes = createMeshFromAssimp(spider_element.scene, spider_element.textureIds);

		spider_element.translation = { 0.0F, 0.0F, 0.0F }; //Starting position
		spider_element.rotation = { 0.0F, 0.0F, 1.0F, 0.0F };
		spider_element.scale = { scaleFactor, scaleFactor, scaleFactor };
		spider_element.usingAssimp = true;
		spider_element.normalMapKey = true;
		spider_node = ScenegraphNode(&spider_element, &shader, 0);
		scenegraph.addNode(&spider_node);
	}

	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearStencil(0x0);
	glEnable(GL_STENCIL_TEST);
}

// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char** argv) {

	//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_MULTISAMPLE);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100, 100);
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
	glutMouseWheelFunc(mouseWheel);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);


	//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
		return(1);

	init();

	renderScene();
	//  GLUT main loop
	glutMainLoop();

	return(0);
}

unsigned int getTextureId(char* name) {
	int i;

	for (i = 0; i < NTEXTURES; ++i)
	{
		if (strncmp(name, flareTextureNames[i], strlen(name)) == 0)
			return i;
	}
	return -1;
}

void loadFlareFile(FLARE_DEF* flare, char* filename)
{
	int     n = 0;
	FILE* f;
	char    buf[256];
	int fields;

	memset(flare, 0, sizeof(FLARE_DEF));

	f = fopen(filename, "r");
	if (f)
	{
		fgets(buf, sizeof(buf), f);
		sscanf(buf, "%f %f", &flare->fScale, &flare->fMaxSize);

		while (!feof(f))
		{
			char            name[8] = { '\0', };
			double          dDist = 0.0, dSize = 0.0;
			float			color[4];
			int				id;

			fgets(buf, sizeof(buf), f);
			fields = sscanf(buf, "%4s %lf %lf ( %f %f %f %f )", name, &dDist, &dSize, &color[3], &color[0], &color[1], &color[2]);
			if (fields == 7)
			{
				for (int i = 0; i < 4; ++i) color[i] = clamp(color[i] / 255.0f, 0.0f, 1.0f);
				id = getTextureId(name);
				if (id < 0) printf("Texture name not recognized\n");
				else
					flare->element[n].textureId = id;
				flare->element[n].fDistance = (float)dDist;
				flare->element[n].fSize = (float)dSize;
				memcpy(flare->element[n].matDiffuse, color, 4 * sizeof(float));
				++n;
			}
		}

		flare->nPieces = n;
		fclose(f);
	}
	else printf("Flare file opening error\n");
}



