#include "Camera.h"
#include <iostream>
#include <cmath>

namespace tankwars {
	Camera::Camera(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
            : eye(eye),
              center(center),
              up(up) {
        // Do nothing
    }

	void Camera::update(const glm::vec3& eye, const glm::vec3&center, const glm::vec3& up) {
		this->eye = eye;
		this->center = center;
		this->up = up;
	}
	void Camera::rotateXAxis(double angle) {					// later the eye will have to rotate around the center ( center being the tank model)
		glm::mat3x3 rot = {	{ glm::cos(-angle),					   0,					-glm::sin(-angle) },
							{				 0,					   1,								   0 },
							{ glm::sin(-angle),					   0,					 glm::cos(-angle)}};
		eye = (rot*(eye- center)) + center;
	}
	void Camera::rotateYAxis(double angle) {					// later the eye will have to rotate around the center ( center being the tank model)
		glm::vec3 rotationAxis = glm::cross(glm::vec3(eye.x - center.x, 0, eye.z - center.z), up);
		glm::vec3 translatedEye = eye - center;
	}

	glm::mat4 Camera::getViewMatrix() const {
		return glm::lookAt(eye, center, up);
	}

	glm::vec3 Camera::getPosition() const {				//For testing puposes
		return center;
	}

	void Camera::move(int direction, float speed) {
		glm::vec3 viewDirection(eye.x - center.x , 0, eye.z - center.z);
		viewDirection = glm::normalize(viewDirection);

		if (direction == 0) {			//FORWARD
			viewDirection = -viewDirection;
		}
		else if (direction == 2) {		//LEFT
			viewDirection = glm::cross(viewDirection, up);
		}
		else if (direction == 3) {		//RIGHT
			viewDirection = -glm::cross(viewDirection, up);
		}
		else {							//BACKWARD schouldn't do anything in this case
			
		}

		eye			= eye + speed * viewDirection;
		center		= center + speed * viewDirection;
	}
}