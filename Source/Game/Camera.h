#pragma once

#include <glm/glm.hpp>

namespace tankwars {
	class Camera {
	public:
        Camera();
		Camera(float fovY, float aspectRatio, float nearZ, float farZ);

        // Call once per frame (or after something changes)
        void update();

        // Makes the camera point at the given target
        //   update() needs to be called after this call
        void lookAt(const glm::vec3& target, const glm::vec3& up);

        // Rotates all axes by the rotation
        //   update() needs to be called after this call
        void rotate(const glm::quat& rotation);

        void setAxes(const glm::vec3& forward, const glm::vec3& up, const glm::vec3& right);
        void setAxes(const glm::quat& axesRotation);

        // Make sure to call update after changing something before accessing these
        const glm::vec3& getRight() const;
        const glm::mat4& getViewMatrix() const;
        const glm::mat4& getProjMatrix() const;
        const glm::mat4& getViewProjMatrix() const;
        
        // update() needs to called after chaning any of these
        float fovY;
        float aspectRatio;
        float nearZ;
        float farZ;
        glm::vec3 position{ 0, 0, 0 };
		glm::vec3 direction{ 0, 0, -1 };
        glm::vec3 up{ 0, 1, 0 };

	private:
		glm::vec3 right{ 1, 0, 0 };
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
        glm::mat4 viewProjMatrix;
	};
}