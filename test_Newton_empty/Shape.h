#pragma once

#include "Newton.h"
#include "Myway.h"


namespace Myway {

	namespace Newton {

		typedef NewtonCollision ntShape;

		// ��������
		ntShape * CreateBox(int shapeId, const Vec3 & vMin, const Vec3 & vMax, const Mat4 & worldTm = Mat4::Identity);

		// ��������
		ntShape * CreateSphere(int shapeId, float radius, const Mat4 & worldTm = Mat4::Identity);

		// ����Բ׶��
		ntShape * CreateCone(int shapeId, float radius, float height, const Mat4 & worldTm = Mat4::Identity);

		// ����������
		ntShape * CreateCapsule(int shapeId, float radius, float height, const Mat4 & worldTm = Mat4::Identity);
		
		// ����Բ��
		ntShape * CreateCylinder(int shapeId, float radius, float height, const Mat4 & worldTm = Mat4::Identity);

		// ��������
		ntShape * CreateMesh(int shapeId, const Vec3 * verts, const int * faces, int numFaces, const Mat4 & worldTm = Mat4::Identity);

		// ��������
		ntShape * CreateTerrain(int shapeId, float * hightFields, float width, float height);
	};
}

