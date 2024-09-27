#ifndef AABB_H
#define AABB_H

#include <vector>
#include <limits>

class AABB {
public:
	std::vector<float> min = {-1.0F, -1.0F, -1.0F, 1.0F}; // min point of aabb in world coordinates;
	std::vector<float> max = {1.0F, 1.0F, 1.0F, 1.0F}; // max point of aabb in world coordinates;

	void update(float* points1, float* points2);

	void update(float* points);

	static void intersepts();

	static void interseptsOnX();
	static void interseptsOnY();
	static void interseptsOnZ();

	static void getLocalPoints(float* transform, float* localPoints);
};

#endif