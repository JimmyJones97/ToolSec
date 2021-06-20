#ifndef INVERSE_ROTATION_MATRIX_H
#define INVERSE_ROTATION_MATRIX_H
#include "../Math/Rotator.h"
#include "../Math/Plane.h"
#include "../Math/Matrix.h"
#include "../Math/UnrealMathUtility.h"

/** Inverse Rotation matrix */
class FInverseRotationMatrix
	: public FMatrix
{
public:
	/**
	 * Constructor.
	 *
	 * @param Rot rotation
	 */
	inline FInverseRotationMatrix(const FRotator& Rot);
};


FInverseRotationMatrix::FInverseRotationMatrix(const FRotator& Rot)
	: FMatrix(
		FMatrix( // Yaw
		FPlane(+FMath::Cos(Rot.Yaw * PI / 180.f), -FMath::Sin(Rot.Yaw * PI / 180.f), 0.0f, 0.0f),
		FPlane(+FMath::Sin(Rot.Yaw * PI / 180.f), +FMath::Cos(Rot.Yaw * PI / 180.f), 0.0f, 0.0f),
		FPlane(0.0f, 0.0f, 1.0f, 0.0f),
		FPlane(0.0f, 0.0f, 0.0f, 1.0f)) *
		FMatrix( // Pitch
		FPlane(+FMath::Cos(Rot.Pitch * PI / 180.f), 0.0f, -FMath::Sin(Rot.Pitch * PI / 180.f), 0.0f),
		FPlane(0.0f, 1.0f, 0.0f, 0.0f),
		FPlane(+FMath::Sin(Rot.Pitch * PI / 180.f), 0.0f, +FMath::Cos(Rot.Pitch * PI / 180.f), 0.0f),
		FPlane(0.0f, 0.0f, 0.0f, 1.0f)) *
		FMatrix( // Roll
		FPlane(1.0f, 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, +FMath::Cos(Rot.Roll * PI / 180.f), +FMath::Sin(Rot.Roll * PI / 180.f), 0.0f),
		FPlane(0.0f, -FMath::Sin(Rot.Roll * PI / 180.f), +FMath::Cos(Rot.Roll * PI / 180.f), 0.0f),
		FPlane(0.0f, 0.0f, 0.0f, 1.0f))
	)
{ }


#endif