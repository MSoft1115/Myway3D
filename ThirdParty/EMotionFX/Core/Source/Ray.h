/*
 * MysticGD Core Library
 * Copyright (c) 2001-2004 Mystic Game Development - http://www.mysticgd.com
 * All Rights Reserved.
 */

#ifndef __RAY_H
#define __RAY_H

//#include <stdio.h>
#include "Macros.h"
#include "Vector.h"
#include "AABB.h"
#include "BoundingSphere.h"
#include "PlaneEq.h"
#include "FastMath.h"
#include "SourceDeclaration.h"
#include "Algorithms.h"


namespace MCore
{

/**
 * The Ray template/class.
 * A ray is normally an infinite line, starting at a given point (the origin) and heading into a given direction.
 * However, this template does not represent an infinite line, but a finite line, since that will be a lot more useful.
 * This means we have an origin, which is the starting point and a destination, which is the end point.
 * This automatically gives us a direction vector too. So basically we now have a finite ray, which is just a 3D line.
 * We can use rays mainly to perform intersection tests. This class provides you with methods to calculate interection information
 * between the ray and bounding spheres, axis aligned bounding boxes, triangles and planes. More intersection tests might be added in a later stage.
 * Or they have already been added, but this documentation should be updated :)
 * Example fields where rays are (often) used are: collision and hit detection, raytracing images, global illumination, lightmap generation, pathtracing,
 * real-time volumetric effects, lensflares, etc.
 */
template <class T>
class TRay
{
	DECLARE_CLASS(Ray<>)

	public:
		/**
		 * Default constructor. Does NOT initialize any members. So this would not be a valid ray.
		 */
		inline TRay<T>() {}

		/**
		 * Constructor which sets the start and end point of the ray.
		 * @param org The origin of the ray.
		 * @param endPoint The end (destination) point of the ray.
		 */
		inline TRay<T>(const TVector3<T>& org, const TVector3<T>& endPoint)										: mOrigin(org), mDest(endPoint), mDirection((endPoint-org).Normalize())	{}

		/**
		 * Constructor which sets the origin, destination point and direction.
		 * @param org The origin of the ray, so where it starts.
		 * @param endPoint The destination point of the ray, so where it should end.
		 * @param dir The normalized direction vector of the ray, which should be (endPoint - startPoint).Normalize()
		 */
		inline TRay<T>(const TVector3<T>& org, const TVector3<T>& endPoint, const TVector3<T>& dir)				: mOrigin(org), mDest(endPoint), mDirection(dir) {}

		/**
		 * Set the origin and destination point (end point) of the ray.
		 * The direction vector will be calculated automatically.
		 * @param org The origin of the ray, so the start point.
		 * @param endPoint The destination of the ray, so the end point.
		 */
		inline void Set(const TVector3<T>& org, const TVector3<T>& endPoint)									{ mOrigin=org; mDest=endPoint; mDirection=(mDest-mOrigin).Normalize(); }

		/**
		 * Set the origin of the ray, so the start point. The direction will automatically be updated as well.
		 * @param org The origin.
		 */
		inline void SetOrigin(const TVector3<T>& org)															{ mOrigin=org; mDirection=(mDest-mOrigin).Normalize(); }

		/**
		 * Set the destination point of the ray.
		 * @param dest The destination of the ray.
		 */
		inline void SetDest(const TVector3<T>& dest)															{ mDest=dest; mDirection=(mDest-mOrigin).Normalize(); }

		/**
		 * Get the origin of the ray.
		 * @result The origin of the ray, so where it starts.
		 */
		inline const TVector3<T>& GetOrigin() const																{ return mOrigin; }

		/**
		 * Get the destination of the ray.
		 * @result The destination point of the ray, so where it ends.
		 */
		inline const TVector3<T>& GetDest() const																{ return mDest; }

		/**
		 * Get the direction of the ray.
		 * @result The normalized direction vector of the ray, so the direction its heading to.
		 */
		inline const TVector3<T>& GetDirection() const															{ return mDirection; }

		/**
		 * Perform a ray/sphere intersection test.
		 * @param s The bounding sphere to test with.
		 * @param intersectA If not NULL, the closest intersection point will be stored in this vector, in case of an intersection.
		 * @param intersectB If not NULL, the farthest intersection point will be stored in this vector, in case of an intersection.
		 * @result Returns true when an intersection occured, otherwise false. If there is no intersection, 'intersectA' and 'intersectB' won't be changed.
		 */
		bool Intersects(const TBoundingSphere<T>& s, TVector3<T> *intersectA=NULL, TVector3<T> *intersectB=NULL) const;

		/**
		 * Perform a ray/plane intersection test.
		 * @param p The plane to test with.
		 * @param intersect If not NULL, the intersection point will be stored in this vector, in case of an intersection.
		 * @result Returns true when an intersection occured, otherwise false. If there is no intersection, 'intersect' will not be changed.
		 */
		bool Intersects(const TPlaneEq<T>& p, TVector3<T> *intersect=NULL) const;

		/**
		 * Perform a ray/triangle intersection test.
		 * @param p1 The first point of the triangle.
		 * @param p2 The second point of the triangle.
		 * @param p3 The third point of the triangle.
		 * @param intersect If not NULL, the intersection point will be stored in here, in case of an intersection.
		 * @param baryU If not NULL, the 'u' barycentric coordinate will be stored in here.
		 * @param baryV If not NULL, the 'v' barycentric coordinate will be stored in here.
		 * @result Returns true in case of an intersection, otherwise false. If there is no intersection, 'intersect', 'baryU' and 'baryV' will not be modified. You
		 * can calculate the uv or normal or whatsoever at the intersection point by using the baryU and baryV values. 
		 * 
		 * The calculation goes like:
		 * 
		 * valueAtIntersectionPoint = (1-u-v)*A + u*B + v*C;
		 *
		 * Where u and v are the values written in baryU and baryV and A, B and C are the three 'attributes' on the 3 points of the triangle. For example the three
		 * vertex normals or uv coordinates or colors. Where A is the attribute linked with 'p1', B the attribute linked with 'p2' and C the attribute linked with 'p3'.
		 *
		 * To make it easy for you, we created a function caled BarycentricInterpolate() which takes the required parameters and returns the attribute value at the given
		 * barycentric coordinates for you. The usage would now be:
		 *
		 * valueAtIntersectionPoint = BarycentricInterpolate<Vector3>(u, v, A, B, C);
		 *
		 * Where A, B and C could be the vertex normals of the triangle for example.
		 * You can easily also calculate the intersection point yourself by using the u and v, by doing this:
		 * 
		 * intersectionPoint = BarycentricInterpolate<Vector3>(u, v, p1, p2, p3);
		 *
		 * @see BarycentricInterpolate
		 */
		bool Intersects(const TVector3<T>& p1, const TVector3<T>& p2, const TVector3<T>& p3, TVector3<T> *intersect=NULL, T *baryU=NULL, T *baryV=NULL) const;

		/**
		 * Perform a ray/AABB (Axis Aligned Bounding Box) intersection test.
		 * @param b The box to test with.
		 * @param intersectA If not NULL, the closest intersection point will be stored in this vector, in case of an intersection.
		 * @param intersectB If not NULL, the farthest intersection point will be stored in this vector, in case of an intersection.
		 * @result Returns true when an intersection occured, otherwise false. If there is no intersection, 'intersectA' and 'intersectB' won't be modified.
		 */
		bool Intersects(const TAABB<T>& b, TVector3<T> *intersectA=NULL, TVector3<T> *intersectB=NULL) const;


	private:
		TVector3<T>		mOrigin;	/**< The origin of the ray. */
		TVector3<T>		mDest;		/**< The destination of the ray. */
		TVector3<T>		mDirection;	/**< The normalized direction vector of the ray. */
};


// include the template code
#include "Ray.inl"

// default types
typedef TRay<MReal>		Ray;
typedef TRay<float>		FRay;
typedef TRay<double>	DRay;

}	// namespace MCore

#endif