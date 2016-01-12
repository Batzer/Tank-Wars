#pragma once

#include "Renderer.h"
#include "Mesh.h"
#include "MeshTools.h"
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

namespace tankwars {
	class Object {
	public:
		Object(Renderer* renderer, btDiscreteDynamicsWorld* world,
            float width, float height, float depth, const glm::vec3& diffuse,
            const btVector3& center, btScalar mass);
        ~Object();

        // Call once per frame, after Bullet updates
		void updateTransform();

    private:
        Renderer* renderer;
        btDiscreteDynamicsWorld* world;
        
        // Rendering
        Mesh mesh;
        Material material;
        MeshInstance meshInstance;

        // Physics
        std::unique_ptr<btCollisionShape> collisionShape;
        std::unique_ptr<btDefaultMotionState> motionState;
        std::unique_ptr<btRigidBody> rigidBody;
	};
}