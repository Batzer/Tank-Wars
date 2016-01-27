#pragma once

#include <btBulletDynamicsCommon.h>
#include <memory>
#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Mesh.h"

namespace tankwars {
    class Object;

	class Physics {
	public:
		Physics();
		void addObject(btVector3 startingPosition, btCollisionShape* shape, btScalar mass, btVector3 fallInertia);
		void transform();
		void removeFromWorld();
		btDiscreteDynamicsWorld* getDynamicsWorld();
	private:
		std::unique_ptr<btBroadphaseInterface> broadphase;
		std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
		std::unique_ptr<btCollisionDispatcher> dispatcher;
		std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
		std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
		std::vector <Object> objects;
	};

	class Object { // move to another file
	public:
		Object(btVector3 startingPosition, btCollisionShape* shape, btScalar mass, btVector3 fallInertia);
		btRigidBody* getRigidBody();

	private:
		MeshInstance mesh;
		std::unique_ptr<btCollisionShape> objShape;
		std::unique_ptr<btDefaultMotionState> objMotionState;
		btRigidBody::btRigidBodyConstructionInfo objRigidBodyCI;
		std::unique_ptr<btRigidBody> objRigidBody;
	};
}