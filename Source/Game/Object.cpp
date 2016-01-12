#include "Object.h"

namespace tankwars {
	Object::Object(Renderer* renderer, btDiscreteDynamicsWorld* world,
                   float width, float height, float depth, const glm::vec3& diffuse,
                   const btVector3& center, btScalar mass)
	        : renderer(renderer),
              world(world),
              mesh(createBoxMesh(height, width, depth)) {
        // Init rendering
        material.diffuseColor = diffuse;
        renderer->addSceneObject(meshInstance);

        // Init physics
        collisionShape.reset(new btBoxShape(btVector3(width / 2, height / 2, depth / 2)));
        motionState.reset(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), center)));

        btVector3 fallInertia;
        collisionShape->calculateLocalInertia(mass, fallInertia);
        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState.get(), collisionShape.get(), fallInertia);
        rigidBody.reset(new btRigidBody(rigidBodyCI));

        world->addRigidBody(rigidBody.get());
	}

    Object::~Object() {
        renderer->removeSceneObject(meshInstance);
        world->removeRigidBody(rigidBody.get());
    }

	void Object::updateTransform() {
        btTransform transform;
		rigidBody->getMotionState()->getWorldTransform(transform);
        /*
        glm::mat4 modelMatrix;
        transform.getOpenGLMatrix(glm::value_ptr(modelMatrix));
		meshInstance.setModelMatrix(modelMatrix);
        */
	}
}