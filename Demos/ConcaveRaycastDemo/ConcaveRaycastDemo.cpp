/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "btBulletDynamicsCommon.h"
#include "LinearMath/btIDebugDraw.h"
#include "GLDebugDrawer.h"
#include "ConcaveRaycastDemo.h"
#include "GL_ShapeDrawer.h"
#include "GlutStuff.h"

//#define USE_PARALLEL_DISPATCHER 1
#ifdef USE_PARALLEL_DISPATCHER
#include "../../Extras/BulletMultiThreaded/SpuGatheringCollisionDispatcher.h"
#include "../../Extras/BulletMultiThreaded/Win32ThreadSupport.h"
#include "../../Extras/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuGatheringCollisionTask.h"
#endif//USE_PARALLEL_DISPATCHER




static btVector3*	gVertices=0;
static int*	gIndices=0;
static btBvhTriangleMeshShape* trimeshShape =0;
static btRigidBody* staticBody = 0;
static float waveheight = 5.f;

const float TRIANGLE_SIZE=8.f;


/* Scrolls back and forth over terrain */
#define NUMRAYS_IN_BAR 100
class btRaycastBar
{
public:
	btVector3 source[NUMRAYS_IN_BAR];
	btVector3 dest[NUMRAYS_IN_BAR];
	btVector3 direction[NUMRAYS_IN_BAR];
	btVector3 hit[NUMRAYS_IN_BAR];
	btVector3 normal[NUMRAYS_IN_BAR];

	int frame_counter;
	int ms;
	int sum_ms;
	int sum_ms_samples;
	int min_ms;
	int max_ms;

	btClock frame_timer;

	btScalar dx;
	btScalar min_x;
	btScalar max_x;
	btScalar min_y;
	btScalar max_y;
	btScalar sign;

	btRaycastBar ()
	{
		ms = 0;
		max_ms = 0;
		min_ms = 9999.0;
		sum_ms_samples = 0;
		sum_ms = 0;
	}

	btRaycastBar (bool unused, btScalar ray_length, btScalar min_z, btScalar max_z, btScalar min_y = -10, btScalar max_y = 10)
	{
		frame_counter = 0;
		ms = 0;
		max_ms = 0;
		min_ms = 9999.0;
		sum_ms_samples = 0;
		sum_ms = 0;
		dx = 10.0;
		min_x = -40;
		max_x = 20;
		this->min_y = min_y;
		this->max_y = max_y;
		sign = 1.0;
		btScalar dalpha = 2*SIMD_2_PI/NUMRAYS_IN_BAR;
		for (int i = 0; i < NUMRAYS_IN_BAR; i++)
		{
			btScalar z = (max_z-min_z)/NUMRAYS_IN_BAR * i + min_z;
			source[i] = btVector3(min_x, max_y, z);
			dest[i] = btVector3(min_x + ray_length, min_y, z);
			normal[i] = btVector3(1.0, 0.0, 0.0);
		}
	}

	btRaycastBar (btScalar ray_length, btScalar z, btScalar min_y = -1000, btScalar max_y = 10)
	{
		frame_counter = 0;
		ms = 0;
		max_ms = 0;
		min_ms = 9999.0;
		sum_ms_samples = 0;
		sum_ms = 0;
		dx = 10.0;
		min_x = -40;
		max_x = 20;
		this->min_y = min_y;
		this->max_y = max_y;
		sign = 1.0;
		btScalar dalpha = 2*SIMD_2_PI/NUMRAYS_IN_BAR;
		for (int i = 0; i < NUMRAYS_IN_BAR; i++)
		{
			btScalar alpha = dalpha * i;
			// rotate around by alpha degrees y 
			btQuaternion q(btVector3(0.0, 1.0, 0.0), alpha);
			direction[i] = btVector3(1.0, 0.0, 0.0);
			direction[i] = q * direction[i];
			direction[i] = direction[i] * ray_length;
			source[i] = btVector3(min_x, max_y, z);
			dest[i] = source[i] + direction[i];
			dest[i][1] = min_y;
			normal[i] = btVector3(1.0, 0.0, 0.0);
		}
	}

	void move (btScalar dt)
	{
		if (dt > (1.0/60.0))
			dt = 1.0/60.0;
		for (int i = 0; i < NUMRAYS_IN_BAR; i++)
		{
			source[i][0] += dx * dt * sign;
			dest[i][0] += dx * dt * sign;
		}
		if (source[0][0] < min_x)
			sign = 1.0;
		else if (source[0][0] > max_x)
			sign = -1.0;
	}

	void cast (btCollisionWorld* cw)
	{
		frame_timer.reset ();
		for (int i = 0; i < NUMRAYS_IN_BAR; i++)
		{
			btCollisionWorld::ClosestRayResultCallback cb(source[i], dest[i]);
			
			cw->rayTest (source[i], dest[i], cb);
			if (cb.HasHit ())
			{
				hit[i] = cb.m_hitPointWorld;
				normal[i] = cb.m_hitNormalWorld;
				normal[i].normalize ();
			} else {
				hit[i] = dest[i];
				normal[i] = btVector3(1.0, 0.0, 0.0);
			}

		}
		ms += frame_timer.getTimeMilliseconds ();
		frame_counter++;
		if (frame_counter > 50)
		{
			min_ms = ms < min_ms ? ms : min_ms;
			max_ms = ms > max_ms ? ms : max_ms;
			sum_ms += ms;
			sum_ms_samples++;
			btScalar mean_ms = (btScalar)sum_ms/(btScalar)sum_ms_samples;
			printf("%d rays in %d ms %d %d %f\n", NUMRAYS_IN_BAR * frame_counter, ms, min_ms, max_ms, mean_ms);
			ms = 0;
			frame_counter = 0;
		}
	}

	void draw ()
	{
		glDisable (GL_LIGHTING);
		glColor3f (0.0, 1.0, 0.0);
		glBegin (GL_LINES);
		for (int i = 0; i < NUMRAYS_IN_BAR; i++)
		{
			glVertex3f (source[i][0], source[i][1], source[i][2]);
			glVertex3f (hit[i][0], hit[i][1], hit[i][2]);
		}
		glEnd ();
		glColor3f (1.0, 1.0, 1.0);
		glBegin (GL_LINES);
		for (int i = 0; i < NUMRAYS_IN_BAR; i++)
		{
			glVertex3f (hit[i][0], hit[i][1], hit[i][2]);
			glVertex3f (hit[i][0] + normal[i][0], hit[i][1] + normal[i][1], hit[i][2] + normal[i][2]);
		}
		glEnd ();
		glColor3f (0.0, 1.0, 1.0);
		glBegin (GL_POINTS);
		for (int i = 0; i < NUMRAYS_IN_BAR; i++)
		{
			glVertex3f (hit[i][0], hit[i][1], hit[i][2]);
		}
		glEnd ();
		glEnable (GL_LIGHTING);
	}
};


static btRaycastBar raycastBar;

///User can override this material combiner by implementing gContactAddedCallback and setting body0->m_collisionFlags |= btCollisionObject::customMaterialCallback;
inline btScalar	calculateCombinedFriction(float friction0,float friction1)
{
	btScalar friction = friction0 * friction1;

	const btScalar MAX_FRICTION  = 10.f;
	if (friction < -MAX_FRICTION)
		friction = -MAX_FRICTION;
	if (friction > MAX_FRICTION)
		friction = MAX_FRICTION;
	return friction;

}

inline btScalar	calculateCombinedRestitution(float restitution0,float restitution1)
{
	return restitution0 * restitution1;
}




const int NUM_VERTS_X = 30;
const int NUM_VERTS_Y = 30;
const int totalVerts = NUM_VERTS_X*NUM_VERTS_Y;

void	ConcaveRaycastDemo::setVertexPositions(float waveheight, float offset)
{
	int i;
	int j;

	for ( i=0;i<NUM_VERTS_X;i++)
	{
		for (j=0;j<NUM_VERTS_Y;j++)
		{
			gVertices[i+j*NUM_VERTS_X].setValue((i-NUM_VERTS_X*0.5f)*TRIANGLE_SIZE,
				//0.f,
				waveheight*sinf((float)i+offset)*cosf((float)j+offset),
				(j-NUM_VERTS_Y*0.5f)*TRIANGLE_SIZE);
		}
	}
}

void ConcaveRaycastDemo::keyboardCallback(unsigned char key, int x, int y)
{
	if (key == 'g')
	{
		m_animatedMesh = !m_animatedMesh;
		if (m_animatedMesh)
		{
			staticBody->setCollisionFlags( staticBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			staticBody->setActivationState(DISABLE_DEACTIVATION);
		} else
		{
			staticBody->setCollisionFlags( staticBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
			staticBody->forceActivationState(ACTIVE_TAG);
		}
	}

	DemoApplication::keyboardCallback(key,x,y);

}

void	ConcaveRaycastDemo::initPhysics()
{
	#define TRISIZE 10.f


	int vertStride = sizeof(btVector3);
	int indexStride = 3*sizeof(int);

	
	const int totalTriangles = 2*(NUM_VERTS_X-1)*(NUM_VERTS_Y-1);

	gVertices = new btVector3[totalVerts];
	gIndices = new int[totalTriangles*3];

	int i;


	setVertexPositions(waveheight,0.f);

	int index=0;
	for ( i=0;i<NUM_VERTS_X-1;i++)
	{
		for (int j=0;j<NUM_VERTS_Y-1;j++)
		{
			gIndices[index++] = j*NUM_VERTS_X+i;
			gIndices[index++] = j*NUM_VERTS_X+i+1;
			gIndices[index++] = (j+1)*NUM_VERTS_X+i+1;

			gIndices[index++] = j*NUM_VERTS_X+i;
			gIndices[index++] = (j+1)*NUM_VERTS_X+i+1;
			gIndices[index++] = (j+1)*NUM_VERTS_X+i;
		}
	}

	m_indexVertexArrays = new btTriangleIndexVertexArray(totalTriangles,
		gIndices,
		indexStride,
		totalVerts,(btScalar*) &gVertices[0].x(),vertStride);

	bool useQuantizedAabbCompression = true;

	trimeshShape  = new btBvhTriangleMeshShape(m_indexVertexArrays,useQuantizedAabbCompression);

	btCollisionShape* groundShape = trimeshShape;
	
	m_collisionConfiguration = new btDefaultCollisionConfiguration();


	m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);


	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	m_broadphase = new btAxisSweep3(worldMin,worldMax);
	m_solver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);

	
	float mass = 0.f;
	btTransform	startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(0,-2,0));

	btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
	m_collisionShapes.push_back(colShape);

	{
		for (int i=0;i<10;i++)
		{
			//btCollisionShape* colShape = new btCapsuleShape(0.5,2.0);//boxShape = new btSphereShape(1.f);
			startTransform.setOrigin(btVector3(2*i,10,1));
			localCreateRigidBody(1, startTransform,colShape);
		}
	}

	startTransform.setIdentity();
	staticBody = localCreateRigidBody(mass, startTransform,groundShape);

	staticBody->setCollisionFlags(staticBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

	//raycastBar = btRaycastBar (4000.0, 0.0);
	raycastBar = btRaycastBar (true, 40.0, -50.0, 50.0);
}

void ConcaveRaycastDemo::clientMoveAndDisplay()
{
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	float dt = m_clock.getTimeMicroseconds() * 0.000001f;
	m_clock.reset();

	if (m_animatedMesh)
	{
		static float offset=0.f;
		offset+=0.01f;

		setVertexPositions(waveheight,offset);

		trimeshShape->refitTree();

		//clear all contact points involving mesh proxy. Note: this is a slow/unoptimized operation.
		m_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(staticBody->getBroadphaseHandle(),getDynamicsWorld()->getDispatcher());
	}

	m_dynamicsWorld->stepSimulation(dt);
	
	raycastBar.move (dt);
	raycastBar.cast (m_dynamicsWorld);
	renderme();
	raycastBar.draw ();
    glFlush();
    glutSwapBuffers();

}




void ConcaveRaycastDemo::displayCallback(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	renderme();
	raycastBar.draw ();
    glFlush();
    glutSwapBuffers();
}



void	ConcaveRaycastDemo::exitPhysics()
{



	//cleanup in the reverse order of creation/initialization

	//remove the rigidbodies from the dynamics world and delete them
	int i;
	for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}

	//delete dynamics world
	delete m_dynamicsWorld;

	if (m_indexVertexArrays)
		delete m_indexVertexArrays;

	//delete solver
	delete m_solver;

	//delete broadphase
	delete m_broadphase;

	//delete dispatcher
	delete m_dispatcher;

	delete m_collisionConfiguration;

	
}



