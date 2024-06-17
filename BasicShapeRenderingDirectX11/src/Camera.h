#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
class Camera
{
public:
	Camera(float screenWidth, float screenHeight);
	~Camera();

	void MouseCallback(double xposIn, double yposIn);
	void ScrollCallback(double xoffset, double yoffset);

	void processInput(GLFWwindow* window, float deltaTime);
	glm::mat4 GetCameraView() const { return mView; }
	glm::mat4 GetCameraProjection() const { return mProjection; }

	void RecalculateViewMatrix();

private:
	glm::mat4 mProjection;
	glm::mat4 mView;

	glm::vec3 mCameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 mCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	bool mFirstMouse = true;
	float mYaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float mPitch = 0.0f;
	float mLastX = 800.0f / 2.0;
	float mLastY = 600.0 / 2.0;
	float mFov = 45.0f;
};