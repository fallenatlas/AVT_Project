#include <vector>

class Camera {
public:
	int type = 0; // 0 - Perspective 1 - Orthogonal
	float offset[3] = { 0.0F, 0.0F, 0.0F }; // rotation around center
	float target[3] = { 0.0F, 0.0F, 0.0F }; // center
	float up[3] = { 0.0F, 1.0F, 0.0F };
	// Camera Spherical Coordinates
	float distance = 10.0F;
	float alpha = 0.0F;
	float beta = 0.0F;

	Camera() {}

	Camera(int t, float dist, float a, float b) : distance(dist), type(t), alpha(a), beta(b) {
		setOffset();
	}

	void setOffset(float a, float b) {
		offset[0] = distance * sin(a * 3.14f / 180.0f) * cos(b * 3.14f / 180.0f);
		offset[2] = distance * cos(a * 3.14f / 180.0f) * cos(b * 3.14f / 180.0f);
		offset[1] = distance * sin(b * 3.14f / 180.0f);
	}

	void setOffset() {
		offset[0] = distance * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
		offset[2] = distance * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
		offset[1] = distance * sin(beta * 3.14f / 180.0f);
	}

	std::vector<float> getPosition() {
		return { target[0] + offset[0], target[1] + offset[1], target[2] + offset[2] };
	}

	void updateProjectionMatrix(float ratio) { 
		loadIdentity(PROJECTION);
		if (type == 0)
			perspective(53.13f, ratio, 0.1f, 1000.0f);
		else
			ortho(-20.0F, 20.0F, -20.0F/ratio, 20.0F/ratio, 0.1F, 50.0F);
	}
};