#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace tankwars {
	class Camera {
	public:
		Camera(glm::vec3 eye, glm::vec3 center, glm::vec3 up);
		~Camera();
		void update(glm::vec3 eye, glm::vec3 center, glm::vec3 up);
		void move(int direction,float alpha);
		void rotate(double angle);
		glm::tmat4x4<float, glm::highp> get();
		glm::vec3 Camera::getCenter();

	private:
		glm::vec3 eye;
		glm::vec3 center;
		glm::vec3 up;
	
	};
}