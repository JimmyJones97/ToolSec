#ifndef PLANE_H
#define PLANE_H

#include "../Math/Vector4.h"

struct FPlane
    : public FVector
{
public:
    float W;

public:

	/** Default constructor (no initialization). */
	inline FPlane();

	/**
	 * Copy Constructor.
	 *
	 * @param P Plane to copy from.
	 */
	inline FPlane(const FPlane& P);

	/**
	 * Constructor.
	 *
	 * @param V 4D vector to set up plane.
	 */
	inline FPlane(const FVector4& V);


    /**
	 * Constructor.
	 *
	 * @param InX X-coefficient.
	 * @param InY Y-coefficient.
	 * @param InZ Z-coefficient.
	 * @param InW W-coefficient.
	 */
	inline FPlane(float InX, float InY, float InZ, float InW);


	/**
	 * Constructor.
	 *
	 * @param InNormal Plane Normal Vector.
	 * @param InW Plane W-coefficient.
	 */
	inline FPlane(FVector InNormal, float InW);

	/**
	 * Constructor.
	 *
	 * @param InBase Base point in plane.
	 * @param InNormal Plane Normal Vector.
	 */
	inline FPlane(FVector InBase, const FVector &InNormal);

	/**
	 * Constructor.
	 *
	 * @param A First point in the plane.
	 * @param B Second point in the plane.
	 * @param C Third point in the plane.
	 */
	inline FPlane(FVector A, FVector B, FVector C);

};


FPlane::FPlane()
{}


FPlane::FPlane(const FPlane& P)
	:	FVector(P)
	,	W(P.W)
{}


FPlane::FPlane(const FVector4& V)
	:	FVector(V)
	,	W(V.W)
{}


FPlane::FPlane(float InX, float InY, float InZ, float InW)
	:	FVector(InX,InY,InZ)
	,	W(InW)
{}

#endif