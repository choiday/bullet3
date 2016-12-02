#pragma once
#include <comm/commtypes.h>
#include <comm/dynarray.h>
#include <comm/hash/hashkeyset.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "BulletCollision/CollisionDispatch/btManifoldResult.h"

#include "physics_cfg.h"

#ifdef PhysX
	#include <PxPhysicsAPI.h>
	#include <PsVecMath.h>
	#include <gjk\GuVecTriangle.h>
	#include <pcm/GuPCMContactGenUtil.h>
	#include <gjk/GuVecBox.h>
	namespace Ps = physx::shdfnd;
#endif
class btConvexPolyhedron;
class btBoxShape;
struct btFace;
class btCollisionWorld;
class btRigidBody;
class btCollisionAlgorithm;


#ifdef PhysX

struct DeferredPolyData
{
public:
	physx::PxVec3	mVerts[3];
	uint32	mInds[3];
	uint32	mTriangleIndex;
	uint32	mFeatureIndex;
	uint8	triFlags;
};
#endif

/// PHYSX STRUCTS AND STUFF

////////////////////////////////////////////////////////////////////////

struct cached_edge {
	uint32 _vId1;
	uint32 _vId2;
	cached_edge()
		:_vId1(-1),
		_vId2(-1)
	{}
	cached_edge(uint32 i1, uint32 i2) {
		_vId1 = glm::max(i1, i2);
		_vId2 = glm::min(i1, i2);
	}
};

template <class VAL,class KEY>
struct edge_keyext {
	typedef KEY ret_type;

	uint32 operator()(const cached_edge & ce) const {
		return (ce._vId1 << 16) | ce._vId2;
	}
};

enum ColliderType {
	ctSphere = 0,
	ctCapsule,
	ctHull,
	ctBox,
	ctCount
};

//
struct contact_point
{
    double3 point;
    float3 normal;
    float depth;
    uint32 t_idx;

    contact_point() {}
    contact_point(const double3 & p,
        const float3 & n,
        float d,
        uint32 idx)
        : point(p)
        , normal(n)
        , depth(d)
        , t_idx(idx)
    {}
};

bool GJK_contact_added(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);
bool friction_combiner_cbk(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);

/// Copied from the Bullet btManifoldResult

float calculate_combined_friction(float b1_fric, float b2_fric);
float calculate_combined_restitution(float b1_rest, float b2_rest);
float calculate_combined_rolling_friction(float b1_roll_fric, float b2_roll_fric);

/// ///////////////

class ot_terrain_contact_common
{
private:
	void prepare(btManifoldResult * result);
	void generateContacts(const glm::vec3 & a, const glm::vec3 & b,
		const glm::vec3 & c, const glm::vec3 & planeNormal,
		const glm::vec3 & normal,
		const glm::vec3 & p,
		const glm::vec3 & q,
		float inflatedRadius);
	void generateEEContacts(const glm::vec3 & a, const glm::vec3 & b,
		const glm::vec3 & c, const glm::vec3 & normal,
		const glm::vec3 & p,
		const glm::vec3 & q,
		float sqInflatedRadius);
	void generateEE(const glm::vec3 & p,
		const glm::vec3 & q,
		float sqInflatedRadius,
		const glm::vec3 & normal,
		const glm::vec3 & a,
		const glm::vec3 & b);

    /// PHYSX STUFF
    void make_fat_edge(float3& p0, float3& p1)
    {
        static const float fat_coef = 0.01f;
        float3 delta = p1 - p0;
        float len = glm::length(delta);

        if (len>0.0f)
        {
            delta *= fat_coef / len;
            p0 -= delta;
            p1 += delta;
        }
    }

    bool intersect_edge_edge(const float3& e00, 
        const float3& e01, 
        const float3& e10, 
        const float3& e11, 
        const float3& dir, 
        float& dist, 
        float3& intersect);

    void generate_ee_contacts_sat2(const float3& a, const float3& b, const float3& c, const float3& normal);
    void generate_ee_contacts_sat(const float3& a, const float3& b, const float3& c, const float3& normal);
    void generate_vf_contacts_sat(const float3& a, const float3& b, const float3& c, const float3& normal);
    bool test_axis_sat(const float3& axis, const float3& a, const float3& b, const float3& c, float& depth);
    bool capsule_tri_overlap_sat(const float3& a,
        const float3& b,
        const float3& c,
        uint8 tflags,
        float* t = NULL,
        float3* pp = NULL);


    ////////////////////////////////////////////////////////////////////////////
#ifdef PhysX

	bool generateTriangleFullContactManifold(physx::Gu::TriangleV& localTriangle,
		const uint32 triangleIndex,
		const uint32* triIndices,
		const uint8 triFlags,
		const btConvexPolyhedron& polyData,
		physx::Gu::SupportLocalImpl<physx::Gu::TriangleV>* localTriMap,
		physx::Gu::SupportLocal* polyMap,
		coid::dynarray<contact_point> & manifoldContacts,
		uint32& numContacts,
		const Ps::aos::FloatVArg contactDist,
		Ps::aos::Vec3V& patchNormal);
	bool testTriangleFaceNormal(const physx::Gu::TriangleV& triangle,
		const btConvexPolyhedron& polyData,
		physx::Gu::SupportLocalImpl<physx::Gu::TriangleV>* triMap,
		physx::Gu::SupportLocal* polyMap,
		const Ps::aos::FloatVArg contactDist,
		Ps::aos::FloatV& minOverlap,
		uint32& feature,
		Ps::aos::Vec3V& faceNormal,
		const physx::Gu::FeatureStatus faceStatus,
		physx::Gu::FeatureStatus& status);
	bool testPolyFaceNormal(const physx::Gu::TriangleV& triangle,
		const btConvexPolyhedron& polyData,
		physx::Gu::SupportLocalImpl<physx::Gu::TriangleV>* triMap,
		physx::Gu::SupportLocal* polyMap,
		const Ps::aos::FloatVArg contactDist,
		Ps::aos::FloatV& minOverlap,
		uint32& feature,
		Ps::aos::Vec3V& faceNormal,
		const physx::Gu::FeatureStatus faceStatus,
		physx::Gu::FeatureStatus& status);
	bool testPolyEdgeNormal(const physx::Gu::TriangleV& triangle,
		const uint8 triFlags,
		const btConvexPolyhedron& /*polygon*/,
		const btConvexPolyhedron& polyData,
		physx::Gu::SupportLocalImpl<physx::Gu::TriangleV>* triMap,
		physx::Gu::SupportLocal* polyMap,
		const Ps::aos::FloatVArg contactDist,
		Ps::aos::FloatV& minOverlap,
		Ps::aos::Vec3V& minNormal,
		const physx::Gu::FeatureStatus edgeStatus,
		physx::Gu::FeatureStatus& status);
	int32 getPolygonIndex(const btConvexPolyhedron& polyData,
		physx::Gu::SupportLocal* map,
		const Ps::aos::Vec3VArg normal);
	void generatedTriangleContacts(const physx::Gu::TriangleV& triangle,
		const uint32 triangleIndex,
		const uint8 _triFlags,
		const btConvexPolyhedron& polyData1,
		const btFace& incidentPolygon,
		physx::Gu::SupportLocal* map1,
		coid::dynarray<contact_point> * manifoldContacts,
		uint32& numContacts,
		const Ps::aos::FloatVArg contactDist,
		const Ps::aos::Vec3VArg contactNormal);
	void generatedPolyContacts(const btConvexPolyhedron& polyData0,
		const btFace& referencePolygon,
		const physx::Gu::TriangleV& triangle,
		const uint32 triangleIndex,
		const uint8 triFlags,
		physx::Gu::SupportLocal* map0,
		coid::dynarray<contact_point> * manifoldContacts,
		uint32& numContacts,
		const Ps::aos::FloatVArg contactDist,
		const Ps::aos::Vec3VArg contactNormal);
	void addContactsToPatch(const Ps::aos::Vec3VArg patchNormal,
		const uint32 previousNumContacts,
		const uint32 _numContacts);
	uint32 reduceContacts(uint32 startIndex, uint32 numPoints);
	void addManifoldPointToPatch(const glm::vec3 currentPatchNormal,
		const float maxPen,
		const uint32 previousNumContacts);
#endif
public:

    COIDNEWDELETE("ot_terrain_contact_common");

	ot_terrain_contact_common(float triangle_collision_margin, btCollisionWorld * world, btCollisionObjectWrapper * planet_body_wrap);
    ~ot_terrain_contact_common();

	void set_terrain_mesh_offset(const glm::dvec3 & offset);
	void prepare_sphere_collision(btManifoldResult * result, const glm::dvec3 & center, float radius, float collision_margin);
	void prepare_capsule_collision(btManifoldResult * result, const glm::dvec3 & p0, const glm::dvec3 & p1,float radius,float collision_margin);
	void prepare_hull_collision(btManifoldResult * result, const glm::quat  & hull_rot, const glm::dvec3 & hull_pos, const btConvexPolyhedron & hull_polydata);
	void prepare_bt_convex_collision(btManifoldResult * result, btCollisionObjectWrapper * convex_object);
    void clear_caches();

	void process_triangle_cache();
	void process_triangle_cache(const coid::dynarray<bt::triangle>& triangle_cache);
	void process_collision_points();
	void process_additional_col_objs();

	void collide_sphere_triangle(const bt::triangle & triangle);
	void collide_capsule_triangle(const bt::triangle & triangle);
    void collide_capsule_triangleSAT(const bt::triangle & triangle);
	void collide_hull_triangle(const bt::triangle & triangle);
	void collide_convex_triangle(const bt::triangle & triangle);

	void add_triangle(const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c, uint32 ia, uint32 ib, uint32 ic, uint8 flags, const double3 * mesh_offset, uint32 tri_idx);
	void add_additional_col_obj(btCollisionObject * col_obj);

    void set_internal_obj_wrapper(btCollisionObjectWrapper * internal_wrap) { _internal_object = internal_wrap;};
    
    void clear_common_data();
private:
	typedef void(ot_terrain_contact_common::*CollisionAlgorithm)(const bt::triangle &);
	btCollisionObjectWrapper * _planet_body_wrap;
	btManifoldResult * _manifold;
	btCollisionWorld * _collision_world;
	coid::dynarray<bt::triangle> _triangle_cache;
	coid::dynarray<contact_point> _contact_point_cache;

	coid::dynarray<btCollisionObject *> _additional_col_objs;

	const float _triangle_collision_margin;
	glm::dvec3 _mesh_offset;

	ColliderType  _curr_collider;
	CollisionAlgorithm _curr_algo;

	float _collider_collision_margin;
	
	glm::dvec3 _sphere_origin_g;
	glm::dvec3 _capsule_p0_g;
	glm::dvec3 _capsule_p1_g;
	glm::dvec3 _hull_center_g;

	glm::vec3 _sphere_origin;
	glm::vec3 _capsule_p0;
	glm::vec3 _capsule_p1;
	
	float _sphere_radius;
	float _capsule_radius;

	const btConvexPolyhedron * _hull_polydata;
	coid::hash_keyset<cached_edge,edge_keyext<cached_edge,uint32> > _cached_edges;

	btCollisionObjectWrapper * _convex_object;
    btCollisionObjectWrapper * _internal_object;
	btTransform _box_local_transform;
	btCollisionAlgorithm * _bt_ca;

#ifdef PhysX
	physx::Gu::BoxV _ps_box;
	physx::Gu::SupportLocal* mPolyMap;
	Ps::aos::PsMatTransformV _mesh_to_covex;
	Ps::aos::PsTransformV	_convex_transform;
	physx::shdfnd::aos::Vec3V _hull_center;
	coid::dynarray<DeferredPolyData> _deferred_data;
#endif
};

