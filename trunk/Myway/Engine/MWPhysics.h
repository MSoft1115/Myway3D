#pragma once

#include "MWMath.h"

namespace Myway
{

namespace Physics
{

struct Material
{
    float   volume;             //���
    float   weight;             //����
    float   friction;           //����
    float   restitution;        //�ظ���
};


enum ShapeType
{
    SHAPE_SPHERE,
    SHAPE_AABB,
    SHAPE_OBB,
    SHAPE_CAPSULE,
    SHAPE_MESH,

    _ALIGN_ENUM(ShapeType)
};

struct Shape;

MW_ENTRY class Actor : public AllocObj
{
public:
    Actor();
    ~Actor();

protected:
    Material            m_material;
    Shape *             m_shape;
};


MW_ENTRY class PhysicsSystem : public Singleton<PhysicsSystem>
{
public:
    PhysicsSystem() {}
    virtual ~PhysicsSystem() {}
};

}

}
