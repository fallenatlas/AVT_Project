#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include "AVTmathLib.h"

class Camera {
public:
	int type = 0; // 0 - Perspective 1 - Orthogonal
	float offset[3] = { 0.0F, 0.0F, 0.0F }; // rotation around center
	float target[3] = { 0.0F, 0.0F, 0.0F }; // center
	float up[3] = { 0.0F, 1.0F, 0.0F };
	// Camera Spherical Coordinates
	float distance = 10.0F;
	float boatAngle = 0.0F;
	float alpha = 0.0F;
	float beta = 0.0F;

	Camera();

	Camera(int t, float dist, float a, float b);

	void setOffset(float a, float b);

	void setOffset();

	std::vector<float> getPosition();

	void updateProjectionMatrix(float ratio);
};

#endif