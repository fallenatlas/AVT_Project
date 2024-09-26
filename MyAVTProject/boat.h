#include <vector>

class Boat {
public:
	float speed = 0.0F;
	float maxSpeed = 10.0F;
	float acceleration = 0.4F;
	float rowing_speed = 10.0F;
	std::vector<float> direction = { 0.0F, 0.0F, 1.0F };

	float speedDecay = 0.99F;
	int elementNum = 0;

	void increaseSpeed(float increase);

	void setDirection(std::vector<float> rotation);
};