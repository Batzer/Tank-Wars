#include "Camera.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace tankwars {
    Camera::Camera()
            : Camera(glm::quarter_pi<float>(), 16.0f / 9.0f, 1.0f, 1000.0f) {
        // Do nothing
    }

    Camera::Camera(float fovY, float aspectRatio, float nearZ, float farZ)
            : fovY(fovY),
              aspectRatio(aspectRatio),
              nearZ(nearZ),
              farZ(farZ) {
        update();
    }

    void Camera::update() {
        right = glm::cross(direction, up);
        viewMatrix = glm::lookAt(position, position + direction, up);
        projMatrix = glm::perspective(fovY, aspectRatio, nearZ, farZ);
        viewProjMatrix = projMatrix * viewMatrix;
    }

    void Camera::lookAt(const glm::vec3& target, const glm::vec3& up) {
        direction = glm::normalize(target - position);
        this->up = up;
    }

    void Camera::rotate(const glm::quat& rotation) {
        direction = rotation * direction;
        up = rotation * up;
        right = rotation * right;
    }

	void Camera::rotatePos(float angle) {
		(void) angle;
		//ho do i rotaaaaate
	}

    void Camera::setAxes(const glm::vec3& forward, const glm::vec3& up, const glm::vec3& right) {
        this->right = right;
        this->up = up;
        direction = forward;
    }

    void Camera::setAxes(const glm::quat& axesRotation) {
        right = axesRotation * glm::vec3(1, 0, 0);
        up = axesRotation * glm::vec3(0, 1, 0);
        direction = axesRotation * glm::vec3(0, 0, -1);
    }

    const glm::vec3& Camera::getRight() const {
        return right;
    }

    const glm::mat4& Camera::getViewMatrix() const {
        return viewMatrix;
    }

    const glm::mat4& Camera::getProjMatrix() const {
        return projMatrix;
    }

    const glm::mat4& Camera::getViewProjMatrix() const {
        return viewProjMatrix;
    }
}