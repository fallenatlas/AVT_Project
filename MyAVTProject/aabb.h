#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

class AABB {
public:
	std::vector<float> center;
	std::vector<float> halfSize{ 1.0F, 1.0F, 1.0F };

	AABB() {}

	void update() {

	}

	void intersepts() {

	}

	void interseptsOnX() {

	}
	void interseptsOnY() {

	}
	void interseptsOnZ() {

	}
};

#endif