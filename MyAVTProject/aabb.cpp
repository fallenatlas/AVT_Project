
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

void AABB::updateWithVec(std::vector<float>& pointsVec) {
	int numPoints = pointsVec.size() / 4;

	for (int i = 0; i < 3; i++) {
		float temp_max = -std::numeric_limits<float>::infinity();
		float temp_min = std::numeric_limits<float>::infinity();
		for (int j = 0; j < numPoints; j++) {
			if (pointsVec[j * 4 + i] > temp_max) {
				temp_max = pointsVec[j * 4 + i];
			}
			if (pointsVec[j * 4 + i] < temp_min) {
				temp_min = pointsVec[j * 4 + i];
			}
		}
		min[i] = temp_min;
		max[i] = temp_max;
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

CollisionInfo AABB::intersepts(AABB other) {
	CollisionInfo collisionInfo{};

	auto infoX = interseptsOnX(other);
	auto infoY = interseptsOnY(other);
	auto infoZ = interseptsOnZ(other);

	// optime, make it exit as soon as the first is false

	collisionInfo.collided = infoX.intersects && infoY.intersects && infoZ.intersects;

	if (collisionInfo.collided) {
		collisionInfo.penetration = infoX.penetration;
		collisionInfo.axis = 0;
		if (infoZ.penetration < collisionInfo.penetration) {
			collisionInfo.penetration = infoZ.penetration;
			collisionInfo.axis = 2;
		}
	}

	return collisionInfo;
}

AxisIntersectionInfo AABB::interseptsOnX(AABB other) {
	AxisIntersectionInfo info{};
	info.intersects = min[0] <= other.max[0] && max[0] >= other.min[0];

	float o = std::min(max[0], other.max[0]) - std::max(min[0], other.min[0]);

	float l = min[0] - other.min[0];
	float r = max[0] - other.max[0];
	if (l * r < 0.0F) // is contained
	{
		o += std::min(abs(l), abs(r));
	}

	info.penetration = o;

	return info;
}

AxisIntersectionInfo AABB::interseptsOnY(AABB other) {
	// for penetration is smalles between the 2 differences
	AxisIntersectionInfo info{};
	info.intersects = min[1] <= other.max[1] && max[1] >= other.min[1];

	float o = std::min(max[1], other.max[1]) - std::max(min[1], other.min[1]);

	float l = min[1] - other.min[1];
	float r = max[1] - other.max[1];
	if (l * r < 0.0F) // is contained
	{
		o += std::min(abs(l), abs(r));
	}

	info.penetration = o;

	return info;
}

AxisIntersectionInfo AABB::interseptsOnZ(AABB other) {
	AxisIntersectionInfo info{};
	info.intersects = min[2] <= other.max[2] && max[2] >= other.min[2];

	float o = std::min(max[2], other.max[2]) - std::max(min[2], other.min[2]);

	float l = min[2] - other.min[2];
	float r = max[2] - other.max[2];
	if (l * r < 0.0F) // is contained
	{
		o += std::min(abs(l), abs(r));
	}

	info.penetration = o;

	return info;
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

void AABB::getGlobalCubePoints(float* transform, std::vector<float>& pointsVec) {
	float points[32];
	float globalPoints[32];
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			points[i * 4 + j] = Cube::vertices[i * 4 + j];
		}
	}

	// apply transformation to points to get them to world coordinates
	for (int i = 0; i < 8; i++) {
		multMatrixPoint(transform, points + (i * 4), globalPoints + (i * 4));
		pointsVec.push_back(globalPoints[i * 4 + 0]);
		pointsVec.push_back(globalPoints[i * 4 + 1]);
		pointsVec.push_back(globalPoints[i * 4 + 2]);
		pointsVec.push_back(globalPoints[i * 4 + 3]);
	}
}

void AABB::getGlobalCilinderPoints(float* transform, std::vector<float>& pointsVec) {
	float radius = 0.4f;
	float height = 2.0f;
	float globalPoints[32];
	float points[] = {
			-radius, height * 0.5f, radius, 1.0F,
			-radius, -height * 0.5f, -radius, 1.0F,
			radius,	height * 0.5f, radius, 1.0F,
			radius, -height * 0.5f, -radius, 1.0F
	};

	for (int i = 0; i < 4; i++) {
		multMatrixPoint(transform, points + (i * 4), globalPoints + (i * 4));
		pointsVec.push_back(globalPoints[i * 4 + 0]);
		pointsVec.push_back(globalPoints[i * 4 + 1]);
		pointsVec.push_back(globalPoints[i * 4 + 2]);
		pointsVec.push_back(globalPoints[i * 4 + 3]);
	}
}