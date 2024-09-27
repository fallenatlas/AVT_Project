
#include "aabb.h"
#include "cube.h"

void multMatrixPoint(float* matrix, float* point, float* res) {

	for (int i = 0; i < 4; ++i) {

		res[i] = 0.0f;

		for (int j = 0; j < 4; j++) {

			res[i] += point[j] * matrix[j * 4 + i];
		}
	}
}

void AABB::update(float* points1, float* points2) {
	// add all points to a single matrix for easier comparing
	float points[64];
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			points[i * 4 + j] = points1[i * 4 + j];
		}
	}
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			points[32 + (i * 4 + j)] = points2[i * 4 + j];
		}
	}

	for (int i = 0; i < 3; i++) {
		float temp_max = -std::numeric_limits<float>::infinity();
		float temp_min = std::numeric_limits<float>::infinity();
		for (int j = 0; j < 16; j++) {
			if (points[j * 4 + i] > temp_max) {
				temp_max = points[j * 4 + i];
			}
			if (points[j * 4 + i] < temp_min) {
				temp_min = points[j * 4 + i];
			}
		}
		min[i] = temp_min;
		max[i] = temp_max;
	}
}

void AABB::update(float* points) {
	// not sure if this works yet
	for (int i = 0; i < 3; i++) {
		float temp_max = -std::numeric_limits<float>::infinity();
		float temp_min = std::numeric_limits<float>::infinity();
		for (int j = 0; j < 8; j++) {
			if (points[j * 4 + i] > temp_max) {
				temp_max = points[j * 4 + i];
			}
			if (points[j * 4 + i] < temp_min) {
				temp_min = points[j * 4 + i];
			}
		}
		min[i] = temp_min;
		max[i] = temp_max;
	}
}

void AABB::intersepts() {

}

void AABB::interseptsOnX() {

}
void AABB::interseptsOnY() {

}
void AABB::interseptsOnZ() {

}

void AABB::getLocalPoints(float* transform, float* localPoints) {
	float points[32];
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			points[i * 4 + j] = Cube::vertices[i * 4 + j];
		}
	}

	// apply transformation to points to get them to world coordinates
	for (int i = 0; i < 8; i++) {
		multMatrixPoint(transform, points + (i * 4), localPoints + (i * 4));
	}
}