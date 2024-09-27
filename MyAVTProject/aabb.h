#ifndef AABB_H
#define AABB_H

#include <vector>
#include <limits>
#include <algorithm>

struct CollisionInfo {
	int axis;
	float penetration;
	bool collided;
};

struct AxisIntersectionInfo {
	bool intersects;
	float penetration;
};

class AABB {
public:
	std::vector<float> min = {-1.0F, -1.0F, -1.0F, 1.0F}; // min point of aabb in world coordinates;
	std::vector<float> max = {1.0F, 1.0F, 1.0F, 1.0F}; // max point of aabb in world coordinates;

	void updateWithVec(std::vector<float>& pointsVec);
	void update(float* points1, float* points2);

	void update(float* points);

	CollisionInfo intersepts(AABB other);

	AxisIntersectionInfo interseptsOnX(AABB other);
	AxisIntersectionInfo interseptsOnY(AABB other);
	AxisIntersectionInfo interseptsOnZ(AABB other);

	static void getLocalPoints(float* transform, float* localPoints);
	static void AABB::getGlobalCubePoints(float* transform, std::vector<float>& pointsVec);
	static void AABB::getGlobalCilinderPoints(float* transform, std::vector<float>& pointsVec);
};

#endif