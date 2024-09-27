#include "camera.h"

Camera::Camera() {}

Camera::Camera(int t, float dist, float a, float b) : distance(dist), type(t), alpha(a), beta(b) {
	setOffset();
}

void Camera::setOffset(float a, float b) {
	a = a + boatAngle;
	offset[0] = distance * sin(a * 3.14f / 180.0f) * cos(b * 3.14f / 180.0f);
	offset[2] = distance * cos(a * 3.14f / 180.0f) * cos(b * 3.14f / 180.0f);
	offset[1] = distance * sin(b * 3.14f / 180.0f);
}

void Camera::setOffset() {
	float a = alpha + boatAngle;
	offset[0] = distance * sin(a * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	offset[2] = distance * cos(a * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	offset[1] = distance * sin(beta * 3.14f / 180.0f);
}

std::vector<float> Camera::getPosition() {
	return { target[0] + offset[0], target[1] + offset[1], target[2] + offset[2] };
}

void Camera::updateProjectionMatrix(float ratio) { 
	loadIdentity(PROJECTION);
	if (type == 0)
		perspective(53.13f, ratio, 0.1f, 1000.0f);
	else
		ortho(-80.0F, 80.0F, -80.0F/ratio, 80.0F/ratio, 0.1F, 50.0F);
}