#include "boat.h"

void Boat::increaseSpeed(float increase) {
	speed += increase;
	if (speed > maxSpeed) {
		speed = maxSpeed;
	}
	else if (speed < -maxSpeed) {
		speed = -maxSpeed;
	}
}

void Boat::setDirection(std::vector<float> rotation) {
	direction = { (float)sin(rotation[0] * 3.14f / 180.0f), 0.0f, (float)cos(rotation[0] * 3.14f / 180.0f) }; // assuming y-axis as the rotation axis
	float mag = sqrt(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);

	direction[0] /= mag;
	direction[1] /= mag;
	direction[2] /= mag;
}