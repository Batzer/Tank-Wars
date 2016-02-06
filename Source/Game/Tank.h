#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>
#include <array>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision\CollisionShapes\btBoxShape.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Wavefront.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "MeshTools.h" // Won't be needed in the final version
#include "Renderer.h"

namespace tankwars {
	class Tank {
	public:
		struct Bullet {
			void set(int own, MeshInstance inst) { owner = own; bulletMeshInstance = inst; }
			void set(btRigidBody* body) { bulletBody.reset(body); }
            bool active = false;
			bool disableMe = false;
			int owner;
			std::unique_ptr<btRigidBody> bulletBody;
			MeshInstance bulletMeshInstance;
		};

		Tank(btDiscreteDynamicsWorld *dynamicsWorld, Renderer& renderer, btVector3 startingPosition, int tankID);

		void addWheels();
		void update(float dt);
		void turn(bool left);
		void drive(bool forward);
		void driveBack(bool backward);
		glm::vec3 getDirectionVector();
		glm::vec3 offset();
		btRaycastVehicle* getAction();
		MeshInstance* getTankMeshInstance(int i);
		glm::vec3 getPosition();
		btRigidBody* getRigidBody();

		//Controller Functions
		void breakController();
		void driveController(bool forward);
		void turnController(float val);
		void turnTurretController(float val);
		void turnHeadAndTurretController(float val);
		void shoot(float dt);
		void adjustPower(bool increase, float dt);
		int tankID;
		void reset(glm::vec3 position, glm::vec3 lookAt);
		void addPoint();
		int getPoints();
	private:
		int points;
        void initializeTankMeshInstances(btVector3 startPos);
		void setTankTuning();

		Renderer* renderer;
        btDiscreteDynamicsWorld* dynamicsWorld;
		std::unique_ptr<btCollisionShape> tankBoxShape;
        std::unique_ptr<btCompoundShape> compoundShape;
		std::unique_ptr<btMotionState> tankMotionState;
        std::unique_ptr<btRigidBody> tankChassis;
        std::unique_ptr<btDefaultVehicleRaycaster> tankVehicleRaycaster;
        std::unique_ptr<btRaycastVehicle> tank;

		btRaycastVehicle::btVehicleTuning tankTuning;
        

		//Tank Meshes and MeshInstances
		glm::mat4x4 tankModelMat;
		Material tankMaterial;
        std::unique_ptr<Mesh> tankBodyMesh;
        std::unique_ptr<Mesh> tankHeadMesh;
        std::unique_ptr<Mesh> tankCanonMesh;
        std::unique_ptr<Mesh> tankLeftFrontWheelMesh;
        std::unique_ptr<Mesh> tankRightFrontWheelMesh;
        std::unique_ptr<Mesh> tankLeftBackWheelMesh;
        std::unique_ptr<Mesh> tankRightBackWheelMesh;

		// 0 = tankBody / 1 = tankHead / 2 = turret / 3-6 = wheels
		std::vector<MeshInstance> tankMeshInstances;					// how to add this shit to the renderer?
		
		//End Tank Meshes and MeshInstances

		// timing Variables
		float lastTimeEngineDecreased = 0;
		float timeBetweenEngineDecreases = 0.1f;
		float lastTimeShot = 0;
		float timeBetweenShots = .5f;

		float lastPowerAdjust = 0;
		float timeBetweenPowerAdjusts = 0.3f;
		//Tank Physics Variables

		float engineForceReduceFactor = 100.f; // reduces this amount every 0.1 sec or whatever
		float maxEngineForce = 3000.f;
		float defaultBreakingForce = 10.f;
		float maxBreakingForce = 100.f;					// what is this for?0.o
		float tankEngineForce = 0.f;
		float tankBreakingForce = 0.f;// defaultBreakingForce;

		float	steeringIncrement = 0.04f;
		float	steeringClamp = 0.2f;
		float	tankSteering = 0.f;

		btScalar mass = 700;
		btScalar wheelWidth = 0.4f;
		btScalar frontWheelRadius = .5f;//0.838f;
		btScalar backWheelRadius = .5f; //1.f;
		btScalar suspensionRestLength = 0.6f;
		btVector3 wheelDirection;
		btVector3 wheelAxle;

		//End Tank Physics Variables

		//Tank Movement Variables
		btScalar shootingPower = 20;				//not adjusted
		btScalar shootingPowerIncrease = 5;		//not adjusted
		btScalar shootingPowerMax = 90;			//not adjusted
		btScalar shootingPowerMin = 10;

		btScalar turretMinAngle = -.1f;
		btScalar turretMaxAngle = .5f;				//not adjusted
		btScalar turretRotationAlpha = .01f;		//not adjusted
		btScalar turretAngle = .0f;
		btScalar headAndTurretRotationAlpha = .01f;	//not adjusted
		btScalar headAndTurretAngle = .0f;

		//End Tank Movement Variables
		class BulletHandler {
		public:
			BulletHandler(btDynamicsWorld* dynamicsWorld, Renderer& renderer, int tankId);
			void createNewBullet(btTransform& tr);
			void updateBullets(btScalar dt, btTransform direction);
			void removeBullet(int index);
			void updatePower(btScalar pwr);
		private:
			btVector3 bulletInertia;
			btScalar mass = 20;
			void removeRaycastBullet(int index);
			btScalar power;
			int tankID;
			btDynamicsWorld* dynamicsWorld;
			Renderer& renderer;
			btSphereShape bulletShape;
			Mesh bulletMesh;
			size_t bulletMax = 20;
			std::array<Bullet, 20> bullets;
			size_t bulletRaycastMax = 500;
			std::array<Bullet, 500> raycastBullets;
			btScalar lastTimeBulletRaycastShot = 0;
			btScalar timeBetweenBulletRaycastShots = 0.02f;
			Material bulletMat;
		};
		BulletHandler bulletHandler;
	};
}
