#include <vector>

class Boat {
public:
	float speed = 0.0F;
	float maxSpeed = 10.0F;
	std::vector<float> direction = { 0.0F, 0.0F, 1.0F };
	// we might want the angle so we can rotate the camera too

	float speedDecay = 0.98F;
	int elementNum = 0;

	void increaseSpeed(float increase) {
		speed += increase;
		if (speed > maxSpeed) {
			speed = maxSpeed;
		}
	}

	void setDirection(std::vector<float> rotation) {
		// direction = rotation_matrix * (1.0F, 0.0F, 0.0F)
		direction = { sin(rotation[0] * 3.14f / 180.0f), 0.0f, cos(rotation[0] * 3.14f / 180.0f) }; // assuming y-axis as the rotation axis
		float mag = sqrt(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);

		direction[0] /= mag;
		direction[1] /= mag;
		direction[2] /= mag;
	}
};