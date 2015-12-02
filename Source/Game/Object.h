#pragma once

#include "Material.h"
#include "MeshInstance.h"
#include "Renderer.h"
#include "StaticMesh.h"
#include "MeshTools.h"
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

namespace gp {
	class Object {
	public:
		Object(Renderer* renderer, btDiscreteDynamicsWorld* world,
            float width, float height, float depth, const glm::vec3& diffuse,
            const btVector3& center, btScalar mass);
        ~Object();

        // Call once per frame, after Bullet updates
		void updateTransform();

    private:
        Renderer* _renderer;
        btDiscreteDynamicsWorld* _world;
        
        // Rendering
        StaticMesh _mesh;
        Material _material;
        MeshInstance _meshInstance;

        // Physics
        std::unique_ptr<btCollisionShape> _collisionShape;
        std::unique_ptr<btDefaultMotionState> _motionState;
        std::unique_ptr<btRigidBody> _rigidBody;
	};
}