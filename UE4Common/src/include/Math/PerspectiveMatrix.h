#ifndef PERSPECTIVE_MATRIX_H
#define PERSPECTIVE_MATRIX_H
#include "Math/Matrix.h"
#include "Math/UnrealMathUtility.h"

class FReversedZPerspectiveMatrix : public FMatrix
{
public:
	FReversedZPerspectiveMatrix(float HalfFOVX, float HalfFOVY, float MultFOVX, float MultFOVY, float MinZ, float MaxZ);
	FReversedZPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ, float MaxZ);
	FReversedZPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ);
};


FReversedZPerspectiveMatrix::FReversedZPerspectiveMatrix(float HalfFOVX, float HalfFOVY, float MultFOVX, float MultFOVY, float MinZ, float MaxZ)
	: FMatrix(
		FPlane(MultFOVX / FMath::Tan(HalfFOVX),	0.0f,								0.0f,													0.0f),
		FPlane(0.0f,							MultFOVY / FMath::Tan(HalfFOVY),	0.0f,													0.0f),
		FPlane(0.0f,							0.0f,								((MinZ == MaxZ) ? 0.0f : MinZ / (MinZ - MaxZ)),			1.0f),
		FPlane(0.0f,							0.0f,								((MinZ == MaxZ) ? MinZ : -MaxZ * MinZ / (MinZ - MaxZ)),	0.0f)
	)
{ }


FReversedZPerspectiveMatrix::FReversedZPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ, float MaxZ)
	: FMatrix(
		FPlane(1.0f / FMath::Tan(HalfFOV),	0.0f,									0.0f,													0.0f),
		FPlane(0.0f,						Width / FMath::Tan(HalfFOV) / Height,	0.0f,													0.0f),
		FPlane(0.0f,						0.0f,									((MinZ == MaxZ) ? 0.0f : MinZ / (MinZ - MaxZ)),			1.0f),
		FPlane(0.0f,						0.0f,									((MinZ == MaxZ) ? MinZ : -MaxZ * MinZ / (MinZ - MaxZ)),	0.0f)
	)
{ }


FReversedZPerspectiveMatrix::FReversedZPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ)
	: FMatrix(
		FPlane(1.0f / FMath::Tan(HalfFOV),	0.0f,									0.0f, 0.0f),
		FPlane(0.0f,						Width / FMath::Tan(HalfFOV) / Height,	0.0f, 0.0f),
		FPlane(0.0f,						0.0f,									0.0f, 1.0f),
		FPlane(0.0f,						0.0f,									MinZ, 0.0f)
	)
{ }

#endif