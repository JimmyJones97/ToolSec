#ifndef UNREAL_MATH_FPU_H
#define UNREAL_MATH_FPU_H
#include "../GenericPlatform/GenericPlatformMemory.h"

typedef unsigned char uint8;

/**
 *	float4 vector register type, where the first float (X) is stored in the lowest 32 bits, and so on.
 */
struct VectorRegister
{
	float	V[4];
};

/**
*	int32[4] vector register type, where the first int32 (X) is stored in the lowest 32 bits, and so on.
*/
struct VectorRegisterInt
{
	int	V[4];
};

/**
*	double[2] vector register type, where the first double (X) is stored in the lowest 64 bits, and so on.
*/
struct VectorRegisterDouble
{
	double	V[2];
};


/**
 * Returns a vector based on 4 FLOATs.
 *
 * @param X		1st float component
 * @param Y		2nd float component
 * @param Z		3rd float component
 * @param W		4th float component
 * @return		Vector of the 4 FLOATs
 */
VectorRegister MakeVectorRegister( float X, float Y, float Z, float W )
{
	VectorRegister Vec = { { X, Y, Z, W } };
	return Vec;
}

/**
 * Replicates one element into all four elements and returns the new vector.
 *
 * @param Vec			Source vector
 * @param ElementIndex	Index (0-3) of the element to replicate
 * @return				VectorRegister( Vec[ElementIndex], Vec[ElementIndex], Vec[ElementIndex], Vec[ElementIndex] )
 */
#define VectorReplicate( Vec, ElementIndex )	MakeVectorRegister( (Vec).V[ElementIndex], (Vec).V[ElementIndex], (Vec).V[ElementIndex], (Vec).V[ElementIndex] )


/**
 * Loads 4 FLOATs from aligned memory.
 *
 * @param Ptr	Aligned memory pointer to the 4 FLOATs
 * @return		VectorRegister(Ptr[0], Ptr[1], Ptr[2], Ptr[3])
 */
#define VectorLoadAligned( Ptr )		MakeVectorRegister( ((const float*)(Ptr))[0], ((const float*)(Ptr))[1], ((const float*)(Ptr))[2], ((const float*)(Ptr))[3] )

/**
 * Stores a vector to aligned memory.
 *
 * @param Vec	Vector to store
 * @param Ptr	Aligned memory pointer
 */
#define VectorStoreAligned( Vec, Ptr )	FGenericPlatformMemory::Memcpy( Ptr, &(Vec), 16 )

/**
 * Converts the 4 FLOATs in the vector to 4 BYTEs, clamped to [0,255], and stores to unaligned memory.
 * IMPORTANT: You need to call VectorResetFloatRegisters() before using scalar FLOATs after you've used this intrinsic!
 *
 * @param Vec			Vector containing 4 FLOATs
 * @param Ptr			Unaligned memory pointer to store the 4 BYTEs.
 */
void VectorStoreByte4( const VectorRegister& Vec, void* Ptr )
{
	uint8 *BytePtr = (uint8*) Ptr;
	BytePtr[0] = uint8( Vec.V[0] );
	BytePtr[1] = uint8( Vec.V[1] );
	BytePtr[2] = uint8( Vec.V[2] );
	BytePtr[3] = uint8( Vec.V[3] );
}



/**
 * Multiplies two vectors (component-wise) and returns the result.
 *
 * @param Vec1	1st vector
 * @param Vec2	2nd vector
 * @return		VectorRegister( Vec1.x*Vec2.x, Vec1.y*Vec2.y, Vec1.z*Vec2.z, Vec1.w*Vec2.w )
 */
VectorRegister VectorMultiply( const VectorRegister& Vec1, const VectorRegister& Vec2 )
{
	VectorRegister Vec;
	Vec.V[0] = Vec1.V[0] * Vec2.V[0];
	Vec.V[1] = Vec1.V[1] * Vec2.V[1];
	Vec.V[2] = Vec1.V[2] * Vec2.V[2];
	Vec.V[3] = Vec1.V[3] * Vec2.V[3];
	return Vec;
}


/**
 * Multiplies two vectors (component-wise), adds in the third vector and returns the result.
 *
 * @param Vec1	1st vector
 * @param Vec2	2nd vector
 * @param Vec3	3rd vector
 * @return		VectorRegister( Vec1.x*Vec2.x + Vec3.x, Vec1.y*Vec2.y + Vec3.y, Vec1.z*Vec2.z + Vec3.z, Vec1.w*Vec2.w + Vec3.w )
 */
VectorRegister VectorMultiplyAdd( const VectorRegister& Vec1, const VectorRegister& Vec2, const VectorRegister& Vec3 )
{
	VectorRegister Vec;
	Vec.V[0] = Vec1.V[0] * Vec2.V[0] + Vec3.V[0];
	Vec.V[1] = Vec1.V[1] * Vec2.V[1] + Vec3.V[1];
	Vec.V[2] = Vec1.V[2] * Vec2.V[2] + Vec3.V[2];
	Vec.V[3] = Vec1.V[3] * Vec2.V[3] + Vec3.V[3];
	return Vec;
}


/**
 * Calculate Homogeneous transform.
 *
 * @param VecP			VectorRegister 
 * @param MatrixM		FMatrix pointer to the Matrix to apply transform
 * @return VectorRegister = VecP*MatrixM
 */
VectorRegister VectorTransformVector(const VectorRegister&  VecP,  const void* MatrixM )
{
	typedef float Float4x4[4][4];
	union { VectorRegister v; float f[4]; } Tmp, Result;
	Tmp.v = VecP;
	const Float4x4& M = *((const Float4x4*)MatrixM);	

	Result.f[0] = Tmp.f[0] * M[0][0] + Tmp.f[1] * M[1][0] + Tmp.f[2] * M[2][0] + Tmp.f[3] * M[3][0];
	Result.f[1] = Tmp.f[0] * M[0][1] + Tmp.f[1] * M[1][1] + Tmp.f[2] * M[2][1] + Tmp.f[3] * M[3][1];
	Result.f[2] = Tmp.f[0] * M[0][2] + Tmp.f[1] * M[1][2] + Tmp.f[2] * M[2][2] + Tmp.f[3] * M[3][2];
	Result.f[3] = Tmp.f[0] * M[0][3] + Tmp.f[1] * M[1][3] + Tmp.f[2] * M[2][3] + Tmp.f[3] * M[3][3];

	return Result.v;
}

#endif