#ifndef MATRIX_H
#define MATRIX_H
#include <string>
#include "Plane.h"
#include "Vector4.h"

struct FMatrix{
public:
    float M[4][4];

	// Constructors.
	FMatrix();

	/**
	 * Constructor.
	 *
	 * @param InX X plane 
	 * @param InY Y plane
	 * @param InZ Z plane
	 * @param InW W plane
	 */
    FMatrix(const FPlane& InX, const FPlane& InY, const FPlane& InZ, const FPlane& InW);

	/**
	 * Gets the result of multiplying a Matrix to this.
	 *
	 * @param Other The matrix to multiply this by.
	 * @return The result of multiplication.
	 */
	FMatrix operator* (const FMatrix& Other) const;

    /** 
	  * This isn't applying SCALE, just multiplying float to all members - i.e. weighting
	  */
	FMatrix operator* (float Other) const;

	// Homogeneous transform.
	FVector4 TransformFVector4(const FVector4& V) const;
};

#endif