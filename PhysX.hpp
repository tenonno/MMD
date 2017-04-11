
#include <PxPhysicsAPI.h>
#include <PxProfileZoneManager.h>

#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "PxPhysicsAPI.h"



// #define PROFILE
// #define CHECKED
#define _PROFILE

#ifdef __CHECKED

#pragma comment (lib, "PhysX3CHECKED_x64.lib")
#pragma comment (lib, "PhysX3CharacterKinematicCHECKED_x64.lib")
#pragma comment (lib, "PhysX3CookingCHECKED_x64.lib")
#pragma comment (lib, "PhysX3CommonCHECKED_x64.lib")
#pragma comment (lib, "PhysX3ExtensionsCHECKED.lib")
#pragma comment (lib, "SimulationControllerCHECKED.lib")
#pragma comment (lib, "PhysX3VehicleCHECKED.lib")
#pragma comment (lib, "LowLevelParticlesCHECKED.lib")
#pragma comment (lib, "LowLevelDynamicsCHECKED.lib")
#pragma comment (lib, "LowLevelCHECKED.lib")
#pragma comment (lib, "LowLevelClothCHECKED.lib")
#pragma comment (lib, "LowLevelAABBCHECKED.lib")
#pragma comment (lib, "PhysX3GpuCHECKED_x64.lib")



#pragma comment (lib, "PxPvdSDKCHECKED_x64.lib")
#pragma comment (lib, "PxFoundationCHECKED_x64.lib")
#pragma comment (lib, "PxTaskCHECKED_x64.lib")
#pragma comment (lib, "PsFastXmlCHECKED_x64.lib")
#pragma comment (lib, "PxCudaContextManagerCHECKED_x64.lib")
#endif

#ifdef _PROFILE
#pragma comment (lib, "PhysX3PROFILE_x64.lib")
#pragma comment (lib, "PhysX3CharacterKinematicPROFILE_x64.lib")
#pragma comment (lib, "PhysX3CookingPROFILE_x64.lib")
#pragma comment (lib, "PhysX3CommonPROFILE_x64.lib")
#pragma comment (lib, "PhysX3ExtensionsPROFILE.lib")
#pragma comment (lib, "SimulationControllerPROFILE.lib")
#pragma comment (lib, "PhysX3VehiclePROFILE.lib")
#pragma comment (lib, "LowLevelParticlesPROFILE.lib")
#pragma comment (lib, "LowLevelDynamicsPROFILE.lib")
#pragma comment (lib, "LowLevelPROFILE.lib")
#pragma comment (lib, "LowLevelClothPROFILE.lib")
#pragma comment (lib, "LowLevelAABBPROFILE.lib")
#pragma comment (lib, "PhysX3GpuPROFILE_x64.lib")



#pragma comment (lib, "PxPvdSDKPROFILE_x64.lib")
#pragma comment (lib, "PxFoundationPROFILE_x64.lib")
#pragma comment (lib, "PxTaskPROFILE_x64.lib")
#pragma comment (lib, "PsFastXmlPROFILE_x64.lib")
#pragma comment (lib, "PxCudaContextManagerPROFILE_x64.lib")



#elif _DEBUG

#pragma comment (lib,"PhysX3DEBUG_x64.lib" )
#pragma comment (lib,"PhysX3CharacterKinematicDEBUG_x64.lib" )
#pragma comment (lib,"PhysX3CookingDEBUG_x64.lib" )
#pragma comment (lib,"PhysX3CommonDEBUG_x64.lib" )
#pragma comment (lib,"PhysX3ExtensionsDEBUG.lib" )
#pragma comment (lib,"SimulationControllerDEBUG.lib" )
#pragma comment (lib,"PhysX3VehicleDEBUG.lib" )
#pragma comment (lib,"LowLevelParticlesDEBUG.lib" )
#pragma comment (lib,"LowLevelDynamicsDEBUG.lib" )
#pragma comment (lib,"LowLevelDEBUG.lib" )
#pragma comment (lib,"LowLevelClothDEBUG.lib" )
#pragma comment (lib,"LowLevelAABBDEBUG.lib" )
#pragma comment (lib,"PhysX3Gpu_x64.lib" )
#pragma comment (lib,"PhysX3GpuCHECKED_x64.lib" )
#pragma comment (lib,"PhysX3GpuDEBUG_x64.lib" )
#pragma comment (lib,"PhysX3GpuPROFILE_x64.lib" )

#else

#pragma comment (lib, "PhysX3_x64.lib")
#pragma comment (lib, "PhysX3CharacterKinematic_x64.lib")
#pragma comment (lib, "PhysX3Cooking_x64.lib")
#pragma comment (lib, "PhysX3Common_x64.lib")
#pragma comment (lib, "PhysX3Extensions.lib")
#pragma comment (lib, "SimulationController.lib")
#pragma comment (lib, "PhysX3Vehicle.lib")
#pragma comment (lib, "LowLevelParticles.lib")
#pragma comment (lib, "LowLevelDynamics.lib")
#pragma comment (lib, "LowLevel.lib")
#pragma comment (lib, "LowLevelCloth.lib")
#pragma comment (lib, "LowLevelAABB.lib")
#pragma comment (lib, "PhysX3Gpu_x64.lib")
// #pragma comment (lib, "PhysX3GpuCHECKED_x64.lib")
// #pragma comment (lib, "PhysX3GpuDEBUG_x64.lib")
// #pragma comment (lib, "PhysX3GpuPROFILE_x64.lib")

#endif



/*

#pragma comment (lib,"PsFastXmlDEBUG_x64.lib" )
#pragma comment (lib,"PxCudaContextManager_x64.lib" )
#pragma comment (lib,"PxCudaContextManagerCHECKED_x64.lib" )
#pragma comment (lib,"PxCudaContextManagerDEBUG_x64.lib" )
#pragma comment (lib,"PxCudaContextManagerPROFILE_x64.lib" )
#pragma comment (lib,"PxFoundationDEBUG_x64.lib" )
#pragma comment (lib,"PxPvdSDKDEBUG_x64.lib" )
#pragma comment (lib,"PxTaskDEBUG_x64.lib" )


*/

/*

#pragma comment (lib, "PxPvdSDK_x64.lib")
// #pragma comment (lib, "PxPvdSDKDEBUG_x64.lib")

#pragma comment (lib, "PxFoundation_x64.lib")
// #pragma comment (lib, "PxFoundationDEBUG_x64.lib")

#pragma comment (lib, "PxTask_x64.lib")
// #pragma comment (lib, "PxTaskDEBUG_x64.lib")

#pragma comment (lib, "PsFastXml_x64.lib")
// #pragma comment (lib, "PsFastXmlDEBUG_x64.lib")
#pragma comment (lib, "PxCudaContextManager_x64.lib")
// #pragma comment (lib, "PxCudaContextManagerCHECKED_x64.lib")
// #pragma comment (lib, "PxCudaContextManagerDEBUG_x64.lib")
// #pragma comment (lib, "PxCudaContextManagerPROFILE_x64.lib")
*/


using namespace physx;




inline PxVec3 ToPxVec3(const Vec3 &vec)
{
	return PxVec3(
		static_cast<float>(vec.x),
		static_cast<float>(vec.y),
		static_cast<float>(vec.z)
	);
}



PxPhysics *m_physics = nullptr;



// spherical joint limited to an angle of at most pi/4 radians (45 degrees)
PxJoint* createLimitedSpherical(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxSphericalJoint* j = PxSphericalJointCreate(*m_physics, a0, t0, a1, t1);
	j->setLimitCone(PxJointLimitCone(PxPi / 4, PxPi / 4, 0.05f));
	j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
	return j;
}

// revolute joint limited to an angle of at most pi/4 radians (45 degrees)

// fixed, breakable joint
PxJoint* createBreakableFixed(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxFixedJoint* j = PxFixedJointCreate(*m_physics, a0, t0, a1, t1);
	j->setBreakForce(1000, 100000);
	j->setConstraintFlag(PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, true);
	return j;
}

// D6 joint with a spring maintaining its position
PxJoint* createDampedD6(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxD6Joint* j = PxD6JointCreate(*m_physics, a0, t0, a1, t1);
	j->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
	j->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
	j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
	j->setDrive(PxD6Drive::eSLERP, PxD6JointDrive(0, 1000, FLT_MAX, true));
	return j;
}


class PhysX_Debugger
{

	PxPvdSceneClient *m_client = nullptr;

public:

	PhysX_Debugger() {}

	PhysX_Debugger(PxPvdSceneClient *client)
		: m_client(client)
	{

	}


	void setCamera(const Camera &camera)
	{

		m_client->updateCamera("Default",
			ToPxVec3(camera.pos),
			ToPxVec3(camera.up),
			ToPxVec3(camera.lookat));

	}


};



struct deleter_physx {
	template<class T>
	void operator()(T *ptr_) {
		ptr_->release();
	}
};

template<class T>
using px_ptr = std::unique_ptr<T, deleter_physx>;



class PhysX
{


	PxFoundation *m_foundation = nullptr;

	PxPvdTransport *m_transport;
	PxPvd *m_pvd;



	PxDefaultAllocator m_allocator;
	PxDefaultErrorCallback m_errorCallback;


	PxMaterial *m_material = nullptr;

public:

	PxScene *m_scene = nullptr;


	template<class T>
	T *create()
	{
		static_assert("a");
	};
	template<class T>
	T *create(const PxTransform &)
	{
		static_assert("a");
	};


	template<>
	PxRigidDynamic *create()
	{
		return create<PxRigidDynamic>(PxTransform(PxVec3(PxZero)));
	}

	template<>
	PxRigidDynamic *create(const PxTransform &transform)
	{
		return m_physics->createRigidDynamic(transform);
	}
	




	template<class T>
	void createChain(PxScene *gScene, PxMaterial *gMaterial, const PxTransform& t, PxU32 length, const PxGeometry& g, PxReal separation, const T createJoint)
	{
		PxVec3 offset(separation / 2, 0, 0);
		PxTransform localTm(offset);
		PxRigidDynamic* prev = NULL;

		for (PxU32 i = 0;i<length;i++)
		{
			PxRigidDynamic* current = PxCreateDynamic(*m_physics, t*localTm, g, *gMaterial, 1.0f);
			(*createJoint)(prev, prev ? PxTransform(offset) : t, current, PxTransform(-offset));
			gScene->addActor(*current);
			prev = current;
			localTm.p.x += separation;
		}
	}

	PhysX_Debugger debugger;

	PxMaterial &getMaterial()
	{
		return *m_material;
	}
	
	PxScene &getScene()
	{
		return *m_scene;
	}

	PxCooking *m_cooking = nullptr;


	PxPvdSceneClient *pvdSceneClient = nullptr;

	PhysX()
	{

		using namespace physx;



		m_foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, m_allocator, m_errorCallback);

		if (!m_foundation) $(L"PxCreateFoundation failed!");

		bool recordMemoryAllocations = true;



		m_pvd = PxCreatePvd(*m_foundation);
		
		m_transport = PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
		if (!m_transport) $(L"");
		
		m_pvd->connect(*m_transport, PxPvdInstrumentationFlag::eALL);


	






		auto scale = PxTolerancesScale();

		m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation,
			scale, recordMemoryAllocations, m_pvd);

		if (!m_physics) $(L"PxCreatePhysics failed!");

		

		m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, PxCookingParams(scale));
		
		if (!m_cooking) $(L"PxCreateCooking failed!");


		PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		auto gDispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = gDispatcher;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		m_scene = m_physics->createScene(sceneDesc);



		m_material = m_physics->createMaterial(0.5f, 0.5f, 0.6f);



		pvdSceneClient = m_scene->getScenePvdClient();



		debugger = PhysX_Debugger(pvdSceneClient);

		if (pvdSceneClient)
		{
			pvdSceneClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdSceneClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdSceneClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}


		connectDebugger();

		/*

		PxRigidStatic* groundPlane = PxCreatePlane(*m_physics, PxPlane(0, 1, 0, 0), *m_material);
		m_scene->addActor(*groundPlane);

		*/

		m_scene->getScenePvdClient()->updateCamera("Def", ToPxVec3(RandomVec3()), PxVec3(0.f, 1.f, 0.f), PxVec3(0.f));

		/*
		createChain(m_scene, gMaterial, PxTransform(PxVec3(0.0f, 20.0f, 0.0f)), 5, PxBoxGeometry(2.0f, 0.5f, 0.5f), 4.0f, &createLimitedSpherical);
		createChain(m_scene, gMaterial, PxTransform(PxVec3(0.0f, 20.0f, -10.0f)), 5, PxBoxGeometry(2.0f, 0.5f, 0.5f), 4.0f, &createBreakableFixed);
		createChain(m_scene, gMaterial, PxTransform(PxVec3(0.0f, 20.0f, -20.0f)), 5, PxBoxGeometry(2.0f, 0.5f, 0.5f), 4.0f, &createDampedD6);
		*/

	}



	void connectDebugger()
	{

	}





	PxTriangleMesh *aaa(Array<Vec3> &vertices, Array<uint32> &indices)
	{

		Array<PxVec3> verts;
		Array<PxU32> indices32;

		verts.reserve(vertices.size());
		indices32.reserve(indices.size());

		for (auto &vertex : vertices)
		{
			verts.emplace_back(PxVec3(
				(float)vertex.x,
				(float)vertex.y,
				(float)vertex.z
			));
		}	
		
		for (auto &index : indices)
		{
			indices32.push_back(index);
		}




		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = verts.size();
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = verts.data();

		meshDesc.triangles.count = indices32.size() / 3;
		meshDesc.triangles.stride = 3 * sizeof(PxU32);
		meshDesc.triangles.data = indices32.data();

		PxDefaultMemoryOutputStream writeBuffer;
		bool status = m_cooking->cookTriangleMesh(meshDesc, writeBuffer);


		if (!status)
		{
			$(L"cook error");
			return nullptr;
		}

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		return m_physics->createTriangleMesh(readBuffer);
	}







	~PhysX()
	{

		m_scene->release();

		m_cooking->release();

		m_physics->release();

		auto transport = m_pvd->getTransport();

		m_pvd->release();
		transport->release();

		m_foundation->release();

	}

	bool isFirstFrame = true;

	void update()
	{

		// PX_UNUSED(true);

		PxReal timeStep = 1.0 / 60.0f;
		//update the kinematice target pose in parallel with collision running
		m_scene->collide(timeStep);
		// updateKinematics(timeStep);
		m_scene->fetchCollision(true);
		m_scene->advance();
		m_scene->fetchResults(true);


	}


};








/*
PxPhysics *gPhysics = nullptr;



PxDefaultAllocator		 gAllocator;
PxDefaultErrorCallback	 gErrorCallback;

PxFoundation*			 gFoundation = NULL;
PxCooking*				 gCooking = NULL;

PxDefaultCpuDispatcher*	 gDispatcher = NULL;
PxScene*				 gPhysicsScene = NULL;

PxMaterial*				 gMaterial = NULL;

PxPvd*           gPvd = NULL;



void initPhysX()
{
	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);
	if (!gFoundation) $(L"PxCreateFoundation failed!");


	
	/*
	PxCookingParams cookingParams(scale);
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, gPhysics->getFoundation(), cookingParams);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gPhysicsScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gPhysicsScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
}
*/