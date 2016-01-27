#include "Physics.h"
namespace tankwars {
	Physics::Physics() 
			: broadphase(new btDbvtBroadphase) , 
			collisionConfiguration(new btDefaultCollisionConfiguration()) , 
			dispatcher(new btCollisionDispatcher(collisionConfiguration.get())) ,
			solver(new btSequentialImpulseConstraintSolver) ,
			dynamicsWorld(new btDiscreteDynamicsWorld(dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get())) 
	{

		dynamicsWorld->setGravity(btVector3(0, -10, 0));
		dynamicsWorld->stepSimulation(1 / 60.f, 10);		// or does it have to be called every time?
	}
	btDiscreteDynamicsWorld* Physics::getDynamicsWorld(){
		return dynamicsWorld.get();
	}
	void Physics::addObject(btVector3 startingPosition,btCollisionShape* shape,btScalar mass ,btVector3 fallInertia) {
		//Object obj(startingPosition, shape, mass, fallInertia);
		objects.emplace_back(startingPosition, shape, mass, fallInertia);
		dynamicsWorld->addRigidBody(objects.back().getRigidBody());//is back always the last added?
		//return objects.back; to identify a certain object??
	}
	void Physics::transform() {
		/*btTransform trans;
		glm::mat4 modelMat;
		for (auto &obj : objects){
			obj.getRigidBody()->getMotionState()->getWorldTransform(trans);
			trans.getOpenGLMatrix(glm::value_ptr(modelMat));
			//obj->mesh.setModelMatrix(modelMat);					similar function has to be implemented
		}
        */
	}
	void Physics::removeFromWorld() {
		/*for (auto &obj : objects) {
			dynamicsWorld->removeRigidBody(obj.getRigidBody());
		}*/
	}

	Object::Object(btVector3 startingPosition, btCollisionShape* shape, btScalar mass, btVector3 fallInertia) 
			:objShape(shape), 
			objMotionState(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), startingPosition))),
			objRigidBodyCI(mass, objMotionState.get(), objShape.get(), fallInertia),
			objRigidBody(new btRigidBody(objRigidBodyCI)) {
															//where should the mesh come from?
		objShape->calculateLocalInertia(mass, fallInertia); // is it ok that this comes afterwards?
	}
	btRigidBody * Object::getRigidBody() {
		return objRigidBody.get();
	}
}