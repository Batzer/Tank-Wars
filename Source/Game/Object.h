#pragma once
#include "Material.h"
#include "MeshInstance.h"
#include "Renderer.h"
#include "StaticMesh.h"
#include "MeshTools.h"
#include <btBulletDynamicsCommon.h>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace gp {
	class Object {
	private:
		Renderer* renderer;
		//btDynamicsWorld* dnmcWrld;
		Material mat;
		MeshInstance obj;
		std::unique_ptr<btCollisionShape> objShape;
		std::unique_ptr<btDefaultMotionState> objMotionState;
		btRigidBody::btRigidBodyConstructionInfo objRigidBodyCI;
		std::unique_ptr<btRigidBody> objRigidBody;
		
	public:
		Object(Renderer* renderer, btDiscreteDynamicsWorld* dnmcWrld, float width, float height, float depth, glm::vec3 diffuse, btScalar mass, btVector3 fallInertia);
		void transform(btTransform trans, glm::mat4 modelMat);
		btRigidBody* getRigidBody() { return objRigidBody.get(); }
		~Object();
	};
}