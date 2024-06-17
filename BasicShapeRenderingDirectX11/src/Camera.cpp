#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float screenWidth, float screenHeight)
{
    mProjection = glm::perspective(glm::radians(mFov), screenWidth / screenHeight, 0.1f, 100.0f);
    mView = glm::lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
}

Camera::~Camera()
{
}

void Camera::MouseCallback(double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (mFirstMouse)
    {
        mLastX = xpos;
        mLastY = ypos;
        mFirstMouse = false;
    }

    float xoffset = xpos - mLastX;
    float yoffset = mLastY - ypos; // reversed since y-coordinates go from bottom to top
    mLastX = xpos;
    mLastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    mYaw += xoffset;
    mPitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (mPitch > 89.0f)
        mPitch = 89.0f;
    if (mPitch < -89.0f)
        mPitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    front.y = sin(glm::radians(mPitch));
    front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mCameraFront = glm::normalize(front);

    RecalculateViewMatrix();
}

void Camera::ScrollCallback(double xoffset, double yoffset)
{
    mFov -= (float)yoffset;
    if (mFov < 1.0f)
        mFov = 1.0f;
    if (mFov > 45.0f)
        mFov = 45.0f;
}

void Camera::processInput(GLFWwindow* window, float deltaTime)
{
    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mCameraPos += cameraSpeed * mCameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mCameraPos -= cameraSpeed * mCameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mCameraPos -= glm::normalize(glm::cross(mCameraFront, mCameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mCameraPos += glm::normalize(glm::cross(mCameraFront, mCameraUp)) * cameraSpeed;

    RecalculateViewMatrix();
}

void Camera::RecalculateViewMatrix()
{
    mView = glm::lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
}
