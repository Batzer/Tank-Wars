#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace tankwars {
	class Camera {
	public:
		Camera(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

		void update(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);
		void move(int direction, float speed);
		void rotateXAxis(double angle);
		void rotateYAxis(double angle);
		glm::mat4 getViewMatrix() const;
		glm::vec3 getPosition() const;

	private:
		glm::vec3 eye;
		glm::vec3 center;
		glm::vec3 up;
	};
}