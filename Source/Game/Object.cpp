#include "Object.h"
namespace gp {
	Object::Object(Renderer* renderer,btDiscreteDynamicsWorld* dynamicWorld, float width, float height, float depth, glm::vec3 diffuse,btScalar mass, btVector3 fallInertia)
							: obj(&createBoxMesh(height,width,depth),&mat),
								objShape(new btBoxShape(btVector3(width/2, height/2, depth/2))),
								objMotionState(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 4, 0)))),
								objRigidBodyCI(mass, objMotionState.get(), objShape.get(), fallInertia),
								objRigidBody(new btRigidBody(objRigidBodyCI)){
		mat.diffuse = diffuse;	
		//dnmcWrld = dynamicWorld;
		renderer->addSceneObject(obj);
		objShape->calculateLocalInertia(mass, fallInertia);
		dynamicWorld->addRigidBody(objRigidBody.get());
	}
	void Object::transform(btTransform trans, glm::mat4 modelMat) {
		objRigidBody->getMotionState()->getWorldTransform(trans);
		trans.getOpenGLMatrix(glm::value_ptr(modelMat));
		obj.setModelMatrix(modelMat);
	}
	Object::~Object(){
		
	}
}