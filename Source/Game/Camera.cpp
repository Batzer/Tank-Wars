#include "Camera.h"
#include <iostream>
#include <cmath>

namespace tankwars {
	Camera::Camera(glm::vec3 eye,glm::vec3 center, glm::vec3 up)
            : eye(eye),
              center(center),
              up(up) {
        // Do nothing
    }

	void Camera::update(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
		this->eye		= eye;
		this->center	= center;
		this->up		= up;
	}

	void Camera::rotateXAxis(double angle) {
        // NOTE: Later the eye will have to rotate around the center ( center being the tank model)
		glm::mat3x3 rot(cos(angle), 0,	-sin(angle) ,
                        0,          1,	 0,
						sin(angle), 0,   cos(angle));
		center = (rot * (center - eye)) + eye;
	}

	void Camera::rotateYAxis(double angle) {
        // NOTE: Later the eye will have to rotate around the center ( center being the tank model)
		// how to rotate around arbitary axis
	}

	glm::tmat4x4<float, glm::highp> Camera::get() {
		return glm::lookAt(eye, center, up);
	}

	glm::vec3 Camera::getCenter() {				//For testing puposes
		return center;
	}

	void Camera::move(int direction,float alpha) {
		glm::vec3 viewDirection = { eye.x - center.x , 0 , eye.z - center.z };
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
			;
		}
		eye			= eye + alpha*viewDirection;
		center		= center + alpha*viewDirection;
	}

	Camera::~Camera() {
		//?
	}
}