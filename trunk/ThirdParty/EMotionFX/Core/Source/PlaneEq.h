/*
 * MysticGD Core Library
 * Copyright (c) 2001-2004 Mystic Game Development - http://www.mysticgd.com
 * All Rights Reserved.
 */

#ifndef __PLANEEQ_H
#define __PLANEEQ_H

#include "Macros.h"
#include "Vector.h"
#include "AABB.h"
#include "Array.h"
#include "SourceDeclaration.h"
#include "Algorithms.h"


namespace MCore
{

/**
 * The Plane Equation template.
 * This represents an infinite plane, which is mathematically represented by the following equation:
 * Ax + By + Cz + d = 0. Where ABC is the XYZ of the planes normal and where xyz is a point on the plane.
 * The value d is a constant value, which is precalculated. Now if we put a random point inside this equation, when we
 * already know the normal and the value for d, we can see if the result of Ax + By + Cz + d is 0 or not.
 * If it is 0, this means the point is on the plane. We can also use this to calculate on what side of the plane a point is.
 * This is for example used for constructing BSP trees.
 * So, from this we can conclude that the result of (normal.Dot(point) + d) is the distance from point to the plane, along the planes normal.
 */
template <class T>
class TPlaneEq
{
	DECLARE_CLASS(TPlaneEq<>)

	public:
		/**
		 * Axis aligned planes.
		 * Used to determine the most dominant axis, which can be used for planar mapping.
		 * This is for example used to generate the texture coordinates for lightmaps.
		 */
		enum EPlane
		{
			PLANE_XY = 0,	/**< The XY plane, so where Z is constant. */
			PLANE_XZ = 1,	/**< The XZ plane, so where Y is constant. */
			PLANE_YZ = 2	/**< The YZ plane, so where X is constant. */
		};

		/**
 		 * The default constructor. Does not initialize anything.
		 * So this does not result in a usable/valid plane.
		 */
		inline TPlaneEq<T>() {}

		/**
 		 * Constructor when you know the normal of the plane and a point on the plane.
		 * @param norm The normal of the plane.
		 * @param pnt A point on the plane.
		 */
		inline TPlaneEq<T>(const TVector3<T>& norm, const TVector3<T>& pnt)										: mNormal(norm), mDist(-((norm.x*pnt.x) + (norm.y*pnt.y) + (norm.z*pnt.z))) {}

		/**
 		 * Constructor when you know the normal and the value of d out of the plane equation (Ax + By + Cz + d = 0)
		 * @param norm The normal of the plane
		 * @param d The value of 'd' out of the plane equation.
		 */
		inline TPlaneEq<T>(const TVector3<T>& norm, const T d)													: mNormal(norm), mDist(d) {}

		/**
 		 * Constructor when you know 3 points on the plane (the winding matters here (clockwise vs counter-clockwise)
		 * The normal will be calculated as ((v2-v1).Cross(v3-v1)).Normalize().
		 * @param v1 The first point on the plane.
		 * @param v2 The second point on the plane.
		 * @param v3 The third point on the plane.
		 */
		inline TPlaneEq<T>(const TVector3<T>& v1, const TVector3<T>& v2, const TVector3<T>& v3)					: mNormal((v2-v1).Cross(v3-v1).Normalize()), mDist(-(mNormal.Dot(v1))) {}

		/**
 		 * Calculates and returns the dominant plane.
		 * A dominant plane is an axis aligned plane, so it can be 3 types of planes, one for each axis.
		 * The way this is calculated is by looking at the normal of the plane, and looking which axis of the normal is the most dominant.
		 * Based on this, corresponding axis aligned plane, can be returned.
		 * @result Returns the type of axis aligned plane. This can be aligned to the XY, XZ or YZ plane. See EPlane for the plane types.
		 */
		inline EPlane CalcDominantPlane() const																	{ return (Math::Abs(mNormal.y) > Math::Abs(mNormal.x) ? (Math::Abs(mNormal.z) > Math::Abs(mNormal.y) ? PLANE_XY : PLANE_XZ) : (Math::Abs(mNormal.z) > Math::Abs(mNormal.x) ? PLANE_XY : PLANE_YZ)); }

		/**
 		 * Calculates the distance of a given point to the plane, along the normal.
		 * A mathematical explaination of how this is done can be read in the description of this template/class.
		 * @param v The vector representing the 3D point to use for the calculation.
		 * @result The distance from 'v' to this plane, along the normal of this plane.
		 */
		inline T CalcDistanceTo(const TVector3<T> &v) const														{ return mNormal.Dot(v) + mDist; }

		/**
		 * Construct the plane when the normal of the plane and a point on the plane are known.
		 * @param normal The normal of the plane.
		 * @param pointOnPlane A point on the plane.
		 */
		inline void Construct(const TVector3<T>& normal, const TVector3<T>& pointOnPlane)						{ mNormal=normal; mDist = -((normal.x*pointOnPlane.x) + (normal.y*pointOnPlane.y) + (normal.z*pointOnPlane.z)); }

		/**
		 * Construct the plane when the normal of the plane is known, as well as the value of 'd' in the plane equation (Ax + By + Cz + d = 0)
		 * @param normal The normal of the plane.
		 * @param d The value of 'd' in the above mentioned plane equation.
		 */
		inline void Construct(const TVector3<T>& normal, const T d)												{ mNormal=normal; mDist = d; }

		/**
		 * Construct the plane when you know three points on the plane. The winding of the vertices matters (clockwise vs counter-clockwise).
		 * The normal is calculated as ((v2-v1).Cross(v3-v1)).Normalize()
		 * @param v1 The first point on the plane.
		 * @param v2 The second point on the plane.
		 * @param v3 The third point on the plane.
		 */
		inline void Construct(const TVector3<T>& v1, const TVector3<T>& v2, const TVector3<T>& v3)				{ mNormal=(v2-v1).Cross(v3-v1).Normalize(); mDist=-(mNormal.Dot(v1)); }

		/**
		 * Get the normal of the plane.
		 * @result Returns the normal of the plane.
		 */
		inline const TVector3<T>& GetNormal() const																{ return mNormal; }

		/**
		 * Get the 'd' out of the plane equation (Ax + By + Cz + d = 0).
		 * @result Returns the 'd' from the plane equation.
		 */
		inline const T& GetDist() const																			{ return mDist; }

		/**
 		 * Checks if a given axis aligned bounding box (AABB) is partially above (aka in front) this plane or not.
		 * The Frustum class uses this method to check if a box is partially inside a the frustum or not.
		 * @param box The axis aligned bounding box to perform the test with.
		 * @result Returns true when 'box' is partially (or completely) above the plane or not.
		 */
		inline bool PartiallyAbove(const TAABB<T>& box) const
		{
			TVector3<T> minVec = box.GetMin();
			TVector3<T> maxVec = box.GetMax();
			TVector3<T> testPoint(	IsNegative(mNormal.x) ? minVec.x : maxVec.x,
									IsNegative(mNormal.y) ? minVec.y : maxVec.y,
									IsNegative(mNormal.z) ? minVec.z : maxVec.z);

			return IsPositive( mNormal.Dot(testPoint) + mDist );		
		}

		/**
 		 * Check if a given axis aligned bounding box (AABB) is completely above (aka in front) this plane or not.
		 * The Frustum class uses this method to check if a box is completely inside a the frustum or not.
		 * @param box The axis aligned bounding box to perform the test with.
		 * @result Returns true when 'box' is completely above the plane or not.
		 */
		inline bool CompletelyAbove(const TAABB<T>& box) const
		{
			TVector3<T> minVec = box.GetMin();
			TVector3<T> maxVec = box.GetMax();
			Vector3 testPoint(	IsPositive(mNormal.x) ? minVec.x : maxVec.x,
								IsPositive(mNormal.y) ? minVec.y : maxVec.y,
								IsPositive(mNormal.z) ? minVec.z : maxVec.z);

			return IsPositive( mNormal.Dot(testPoint) + mDist );
		}

		/**
 		 * Clips a set of 3D points to this plane.
		 * Actually these are not just points, but edges. The edges go from point 0 to 1, from 1 to 2, etc.
		 * Beware that the clipped number of points can be higher as the ones you input to this method.
		 * This method can be used to pretty easily clip polygon data against the plane.
		 * @param pointsIn The array of points (and edges) to be clipped to the planes.
		 * @param pointsOut The array of clipped points (and edges). Note that (pointsOut.GetLength() > pointsIn.GetLength()) can be true.
		 * @result Returns true when the points have been clipped. False is returned when the clipping resulted in 0 output points.
		 */
		bool Clip(const Array< TVector3<T> >& pointsIn, Array< TVector3<T> >& pointsOut) const;

		/**
 		 * Clip a set of 3D points to this plane.
		 * Actually these are not just points, but edges. The edges go from point 0 to 1, from 1 to 2, etc.
		 * Beware that the clipped number of points can be higher as the ones you input to this method.
		 * This method can be used to pretty easily clip polygon data against the plane.
		 * @param points The set of points (or edges) to clip. When done, points contains the clipped points.
		 * @result Returns true when the points have been clipped. False is returned when the clipping resulted in 0 points. In that last case 'points' won't be effected and contains just the original input points.
		 */
		bool Clip(Array< TVector3<T> >& points) const;



	private:
		TVector3<T>	mNormal;	/**< The normal of the plane. */
		T			mDist;		/**< The d in the plane equation. (Ax + By + Cz + d = 0)*/
};

// include the template code
#include "PlaneEq.inl"

// default types
typedef TPlaneEq<MReal>		PlaneEq;
typedef TPlaneEq<float>		FPlaneEq;
typedef TPlaneEq<double>	DPlaneEq;

}	// namespace MCore

#endif