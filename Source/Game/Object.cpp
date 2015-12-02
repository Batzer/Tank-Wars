#include "Object.h"

namespace gp {
	Object::Object(Renderer* renderer, btDiscreteDynamicsWorld* world,
                   float width, float height, float depth, const glm::vec3& diffuse,
                   const btVector3& center, btScalar mass)
	    : _renderer(renderer)
        , _world(world)
        , _mesh(createBoxMesh(height, width, depth))
        , _meshInstance(&_mesh, &_material)
    {
        // Init rendering
        _material.diffuse = diffuse;
        _renderer->addSceneObject(_meshInstance);

        // Init physics
        _collisionShape.reset(new btBoxShape(btVector3(width / 2, height / 2, depth / 2)));
        _motionState.reset(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), center)));

        btVector3 fallInertia;
        _collisionShape->calculateLocalInertia(mass, fallInertia);
        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, _motionState.get(), _collisionShape.get(), fallInertia);
        _rigidBody.reset(new btRigidBody(rigidBodyCI));

        _world->addRigidBody(_rigidBody.get());
	}

    Object::~Object() {
        _renderer->removeSceneObject(_meshInstance);
        _world->removeRigidBody(_rigidBody.get());
    }

	void Object::updateTransform() {
        btTransform transform;
		_rigidBody->getMotionState()->getWorldTransform(transform);

        glm::mat4 modelMatrix;
        transform.getOpenGLMatrix(glm::value_ptr(modelMatrix));
		_meshInstance.setModelMatrix(modelMatrix);
	}
}