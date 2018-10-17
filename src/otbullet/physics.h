#pragma once

#ifndef __INTERGEN_GENERATED__physics_H__
#define __INTERGEN_GENERATED__physics_H__

//@file Interface file for physics interface generated by intergen
//See LICENSE file for copyright and license information

#include <comm/commexception.h>
#include <comm/intergen/ifc.h>

#include "physics_cfg.h"
#include <comm/alloc/slotalloc.h>

class btDynamicsWorld;
class btCollisionShape;
class btCompoundShape;
class btCollisionObject;
class btGhostObject;
class btRigidBody;
class btActionInterface;
class btTransform;
class btIDebugDraw;
class btManifoldPoint;

namespace bt {
    class constraint_info;
    class physics;
    struct ot_world_physics_stats;
}
extern bt::physics* BT;

class physics;


namespace bt {

////////////////////////////////////////////////////////////////////////////////
///Interface for the physics module
class physics
    : public intergen_interface
{
public:

    // --- interface methods ---

    void step_simulation( double step );

    void ray_test( const double from[3], const double to[3], void* cb );

    void set_current_frame( uint frame );

    btRigidBody* fixed_object();

    btRigidBody* create_rigid_body( float mass, btCollisionShape* shape, void* usr1, void* usr2 );

    void destroy_rigid_body( btRigidBody*& obj );

    void add_rigid_body( btRigidBody* obj, unsigned int group, unsigned int mask, btActionInterface* action, bt::constraint_info* constraint );

    void remove_rigid_body( btRigidBody* obj, btActionInterface* action, bt::constraint_info* constraint );

    void pause_rigid_body( btRigidBody* obj, bool pause );

    void set_rigid_body_mass( btRigidBody* obj, float mass, const float inertia[3] );

    void set_rigid_body_gravity( btRigidBody* obj, const double gravity[3] );

    void set_rigid_body_transform( btRigidBody* obj, const btTransform& tr, const double gravity[3] );

    void predict_rigid_body_transform( btRigidBody* obj, double dt, ifc_out btTransform& tr );

    btCollisionObject* create_collision_object( btCollisionShape* shape, void* usr1, void* usr2 );

    btGhostObject* create_ghost_object( btCollisionShape* shape, void* usr1, void* usr2 );

    void destroy_collision_object( btCollisionObject*& obj );

    void update_collision_object( btCollisionObject* obj, const btTransform& tr, bool update_aabb );

    void set_collision_info( btCollisionObject* obj, unsigned int group, unsigned int mask );

    void add_collision_object( btCollisionObject* obj, unsigned int group, unsigned int mask, bool inactive );

    void remove_collision_object( btCollisionObject* obj );

    btCompoundShape* create_compound_shape();

    void add_child_shape( btCompoundShape* group, btCollisionShape* child, const btTransform& tr );

    void update_child( btCompoundShape* group, int index, const btTransform& tr );

    void recalc_compound_shape( btCompoundShape* shape );

    void destroy_compound_shape( ifc_inout btCompoundShape*& shape );

    btCollisionShape* create_shape( bt::EShape sh, const float hvec[3] );

    void add_convex_point( btCollisionShape* shape, const float point[3] );

    void close_convex_shape( btCollisionShape* shape );

    void destroy_shape( ifc_inout btCollisionShape*& shape );

    bt::ot_world_physics_stats get_stats();

    bt::ot_world_physics_stats* get_stats_ptr();

    void set_debug_draw_enabled( btIDebugDraw* debug_drawer );

    void set_debug_drawer_mode( int debug_mode );

    void debug_draw_world();

    void query_volume_sphere( const double3& pos, float rad, ifc_inout coid::dynarray<btCollisionObject*>& result );

    void query_volume_frustum( const double3& pos, const float4* f_planes_norms, uint8 nplanes, bool include_partial, ifc_inout coid::dynarray<btCollisionObject *>& result );

    void wake_up_objects_in_radius( const double3& pos, float rad );

    bool is_point_inside_terrain_ocluder( const double3& pt );


protected:
    // --- interface events (callbacks from host to client) ---
    // ---       overload these to handle host events       ---

    friend class ::physics;

    virtual bool terrain_collisions( const void* context, const double3& center, float radius, float lod_dimension, coid::dynarray<bt::triangle>& data, coid::dynarray<uint>& trees, coid::slotalloc<bt::tree_batch>& tree_batches, uint frame ){ throw coid::exception("handler not implemented"); }

    virtual int terrain_collisions_aabb( const void* context, const double3& center, float3x3 basis, float lod_dimension, coid::dynarray<bt::triangle>& data, coid::dynarray<uint>& trees, coid::slotalloc<bt::tree_batch>& tree_batches, uint frame, bool& is_above_tm, double3& under_contact, float3& under_normal ){ throw coid::exception("handler not implemented"); }

    virtual float3 tree_collisions( btRigidBody* obj, bt::tree_collision_contex& ctx, float time_step, coid::slotalloc<bt::tree_batch>& tree_batches ){ throw coid::exception("handler not implemented"); }

    virtual float terrain_ray_intersect( const void* context, const double3& from, const float3& dir, const float2& minmaxlen, float3* norm, double3* pos ){ throw coid::exception("handler not implemented"); }

    virtual float elevation_above_terrain( const double3& pos, float maxlen, float3* norm, double3* hitpoint ){ throw coid::exception("handler not implemented"); }

    virtual void add_static_collider( const void* context, btCollisionObject* obj, const double3& cen, const float3x3& basis ) {}

    virtual void log( const coid::token& text ) {}

    virtual void force_bind_script_events() {}

public:
    // --- host helpers to check presence of handlers in scripts ---

    virtual bool is_bound_terrain_collisions() { return true; }
    virtual bool is_bound_terrain_collisions_aabb() { return true; }
    virtual bool is_bound_tree_collisions() { return true; }
    virtual bool is_bound_terrain_ray_intersect() { return true; }
    virtual bool is_bound_elevation_above_terrain() { return true; }
    virtual bool is_bound_add_static_collider() { return true; }
    virtual bool is_bound_log() { return true; }

public:
    // --- creators ---

    static iref<physics> create( double r, void* context ) {
        return create<physics>(0, r, context);
    }

    template<class T>
    static iref<T> create( T* _subclass_, double r, void* context );

    static iref<physics> get() {
        return get<physics>(0);
    }

    template<class T>
    static iref<T> get( T* _subclass_ );

    // --- internal helpers ---

    virtual ~physics() {
        if (_cleaner) _cleaner(this,0);
    }

    ///Interface revision hash
    static const int HASHID = 98631184;
    
    ///Interface name (full ns::class string)
    static const coid::tokenhash& IFCNAME() {
        static const coid::tokenhash _name = "bt::physics";
        return _name;
    }

    int intergen_hash_id() const override final { return HASHID; }

    bool iface_is_derived( int hash ) const override final {
        return hash == HASHID;
    }

    const coid::tokenhash& intergen_interface_name() const override final {
        return IFCNAME();
    }

    static const coid::token& intergen_default_creator_static( EBackend bck ) {
        static const coid::token _dc("bt::physics.get@98631184");
        static const coid::token _djs("bt::physics@wrapper.js");
        static const coid::token _djsc("bt::physics@wrapper.jsc");
        static const coid::token _dlua("bt::physics@wrapper.lua");
        static const coid::token _dnone;

        switch(bck) {
        case IFC_BACKEND_CXX: return _dc;
        case IFC_BACKEND_JS:  return _djs;
        case IFC_BACKEND_JSC:  return _djsc;
        case IFC_BACKEND_LUA: return _dlua;
        default: return _dnone;
        }
    }


    template<enum EBackend B>
    static void* intergen_wrapper_cache() {
        static void* _cached_wrapper=0;
        if (!_cached_wrapper) {
            const coid::token& tok = intergen_default_creator_static(B);
            _cached_wrapper = coid::interface_register::get_interface_creator(tok);
        }
        return _cached_wrapper;
    }

    void* intergen_wrapper( EBackend bck ) const override final {
        switch(bck) {
        case IFC_BACKEND_JS: return intergen_wrapper_cache<IFC_BACKEND_JS>();
        case IFC_BACKEND_JSC: return intergen_wrapper_cache<IFC_BACKEND_JSC>();
        case IFC_BACKEND_LUA: return intergen_wrapper_cache<IFC_BACKEND_LUA>();
        default: return 0;
        }
    }

    EBackend intergen_backend() const override { return IFC_BACKEND_CXX; }

    const coid::token& intergen_default_creator( EBackend bck ) const override final {
        return intergen_default_creator_static(bck);
    }

    ///Client registrator
    template<class C>
    static int register_client()
    {
        static_assert(std::is_base_of<physics, C>::value, "not a base class");

        typedef iref<intergen_interface> (*fn_client)(void*, intergen_interface*);
        fn_client cc = [](void*, intergen_interface*) -> iref<intergen_interface> { return new C; };

        coid::token type = typeid(C).name();
        type.consume("class ");
        type.consume("struct ");

        coid::charstr tmp = "bt::physics";
        tmp << "@client-98631184" << '.' << type;

        coid::interface_register::register_interface_creator(tmp, cc);
        return 0;
    }

protected:

    typedef void (*cleanup_fn)(physics*, intergen_interface*);
    cleanup_fn _cleaner;

    physics() : _cleaner(0)
    {}
};

////////////////////////////////////////////////////////////////////////////////
template<class T>
inline iref<T> physics::create( T* _subclass_, double r, void* context )
{
    typedef iref<T> (*fn_creator)(physics*, double, void*);

    static fn_creator create = 0;
    static const coid::token ifckey = "bt::physics.create@98631184";

    if (!create)
        create = reinterpret_cast<fn_creator>(
            coid::interface_register::get_interface_creator(ifckey));

    if (!create) {
        log_mismatch("create", "bt::physics.create", "@98631184");
        return 0;
    }

    return create(_subclass_, r, context);
}

////////////////////////////////////////////////////////////////////////////////
template<class T>
inline iref<T> physics::get( T* _subclass_ )
{
    typedef iref<T> (*fn_creator)(physics*);

    static fn_creator create = 0;
    static const coid::token ifckey = "bt::physics.get@98631184";

    if (!create)
        create = reinterpret_cast<fn_creator>(
            coid::interface_register::get_interface_creator(ifckey));

    if (!create) {
        log_mismatch("get", "bt::physics.get", "@98631184");
        return 0;
    }

    return create(_subclass_);
}

#pragma warning(push)
#pragma warning(disable : 4191)

inline void physics::step_simulation( double step )
{ return VT_CALL(void,(double),0)(step); }

inline void physics::ray_test( const double from[3], const double to[3], void* cb )
{ return VT_CALL(void,(const double[3],const double[3],void*),1)(from,to,cb); }

inline void physics::set_current_frame( uint frame )
{ return VT_CALL(void,(uint),2)(frame); }

inline btRigidBody* physics::fixed_object()
{ return VT_CALL(btRigidBody*,(),3)(); }

inline btRigidBody* physics::create_rigid_body( float mass, btCollisionShape* shape, void* usr1, void* usr2 )
{ return VT_CALL(btRigidBody*,(float,btCollisionShape*,void*,void*),4)(mass,shape,usr1,usr2); }

inline void physics::destroy_rigid_body( btRigidBody*& obj )
{ return VT_CALL(void,(btRigidBody*&),5)(obj); }

inline void physics::add_rigid_body( btRigidBody* obj, unsigned int group, unsigned int mask, btActionInterface* action, bt::constraint_info* constraint )
{ return VT_CALL(void,(btRigidBody*,unsigned int,unsigned int,btActionInterface*,bt::constraint_info*),6)(obj,group,mask,action,constraint); }

inline void physics::remove_rigid_body( btRigidBody* obj, btActionInterface* action, bt::constraint_info* constraint )
{ return VT_CALL(void,(btRigidBody*,btActionInterface*,bt::constraint_info*),7)(obj,action,constraint); }

inline void physics::pause_rigid_body( btRigidBody* obj, bool pause )
{ return VT_CALL(void,(btRigidBody*,bool),8)(obj,pause); }

inline void physics::set_rigid_body_mass( btRigidBody* obj, float mass, const float inertia[3] )
{ return VT_CALL(void,(btRigidBody*,float,const float[3]),9)(obj,mass,inertia); }

inline void physics::set_rigid_body_gravity( btRigidBody* obj, const double gravity[3] )
{ return VT_CALL(void,(btRigidBody*,const double[3]),10)(obj,gravity); }

inline void physics::set_rigid_body_transform( btRigidBody* obj, const btTransform& tr, const double gravity[3] )
{ return VT_CALL(void,(btRigidBody*,const btTransform&,const double[3]),11)(obj,tr,gravity); }

inline void physics::predict_rigid_body_transform( btRigidBody* obj, double dt, btTransform& tr )
{ return VT_CALL(void,(btRigidBody*,double,btTransform&),12)(obj,dt,tr); }

inline btCollisionObject* physics::create_collision_object( btCollisionShape* shape, void* usr1, void* usr2 )
{ return VT_CALL(btCollisionObject*,(btCollisionShape*,void*,void*),13)(shape,usr1,usr2); }

inline btGhostObject* physics::create_ghost_object( btCollisionShape* shape, void* usr1, void* usr2 )
{ return VT_CALL(btGhostObject*,(btCollisionShape*,void*,void*),14)(shape,usr1,usr2); }

inline void physics::destroy_collision_object( btCollisionObject*& obj )
{ return VT_CALL(void,(btCollisionObject*&),15)(obj); }

inline void physics::update_collision_object( btCollisionObject* obj, const btTransform& tr, bool update_aabb )
{ return VT_CALL(void,(btCollisionObject*,const btTransform&,bool),16)(obj,tr,update_aabb); }

inline void physics::set_collision_info( btCollisionObject* obj, unsigned int group, unsigned int mask )
{ return VT_CALL(void,(btCollisionObject*,unsigned int,unsigned int),17)(obj,group,mask); }

inline void physics::add_collision_object( btCollisionObject* obj, unsigned int group, unsigned int mask, bool inactive )
{ return VT_CALL(void,(btCollisionObject*,unsigned int,unsigned int,bool),18)(obj,group,mask,inactive); }

inline void physics::remove_collision_object( btCollisionObject* obj )
{ return VT_CALL(void,(btCollisionObject*),19)(obj); }

inline btCompoundShape* physics::create_compound_shape()
{ return VT_CALL(btCompoundShape*,(),20)(); }

inline void physics::add_child_shape( btCompoundShape* group, btCollisionShape* child, const btTransform& tr )
{ return VT_CALL(void,(btCompoundShape*,btCollisionShape*,const btTransform&),21)(group,child,tr); }

inline void physics::update_child( btCompoundShape* group, int index, const btTransform& tr )
{ return VT_CALL(void,(btCompoundShape*,int,const btTransform&),22)(group,index,tr); }

inline void physics::recalc_compound_shape( btCompoundShape* shape )
{ return VT_CALL(void,(btCompoundShape*),23)(shape); }

inline void physics::destroy_compound_shape( btCompoundShape*& shape )
{ return VT_CALL(void,(btCompoundShape*&),24)(shape); }

inline btCollisionShape* physics::create_shape( bt::EShape sh, const float hvec[3] )
{ return VT_CALL(btCollisionShape*,(bt::EShape,const float[3]),25)(sh,hvec); }

inline void physics::add_convex_point( btCollisionShape* shape, const float point[3] )
{ return VT_CALL(void,(btCollisionShape*,const float[3]),26)(shape,point); }

inline void physics::close_convex_shape( btCollisionShape* shape )
{ return VT_CALL(void,(btCollisionShape*),27)(shape); }

inline void physics::destroy_shape( btCollisionShape*& shape )
{ return VT_CALL(void,(btCollisionShape*&),28)(shape); }

inline bt::ot_world_physics_stats physics::get_stats()
{ return VT_CALL(bt::ot_world_physics_stats,(),29)(); }

inline bt::ot_world_physics_stats* physics::get_stats_ptr()
{ return VT_CALL(bt::ot_world_physics_stats*,(),30)(); }

inline void physics::set_debug_draw_enabled( btIDebugDraw* debug_drawer )
{ return VT_CALL(void,(btIDebugDraw*),31)(debug_drawer); }

inline void physics::set_debug_drawer_mode( int debug_mode )
{ return VT_CALL(void,(int),32)(debug_mode); }

inline void physics::debug_draw_world()
{ return VT_CALL(void,(),33)(); }

inline void physics::query_volume_sphere( const double3& pos, float rad, coid::dynarray<btCollisionObject*>& result )
{ return VT_CALL(void,(const double3&,float,coid::dynarray<btCollisionObject*>&),34)(pos,rad,result); }

inline void physics::query_volume_frustum( const double3& pos, const float4* f_planes_norms, uint8 nplanes, bool include_partial, coid::dynarray<btCollisionObject *>& result )
{ return VT_CALL(void,(const double3&,const float4*,uint8,bool,coid::dynarray<btCollisionObject *>&),35)(pos,f_planes_norms,nplanes,include_partial,result); }

inline void physics::wake_up_objects_in_radius( const double3& pos, float rad )
{ return VT_CALL(void,(const double3&,float),36)(pos,rad); }

inline bool physics::is_point_inside_terrain_ocluder( const double3& pt )
{ return VT_CALL(bool,(const double3&),37)(pt); }

#pragma warning(pop)

} //namespace

#endif //__INTERGEN_GENERATED__physics_H__
