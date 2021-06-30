#ifndef VECTOR_H
#define VECTOR_H
//#include "Math/Vector4.h"
#include "../Misc/CoreMiscDefines.h"
#include "IntPoint.h"

class FVector4;

struct FVector
{
public:
    float X;
    float Y;
    float Z;

public:


	/** Default constructor (no initialization). */
	inline FVector();

	/**
	 * Constructor initializing all components to a single float value.
	 *
	 * @param InF Value to set all components to.
	 */
	explicit inline FVector(float InF);

	/**
	 * Constructor using initial values for each component.
	 *
	 * @param InX X Coordinate.
	 * @param InY Y Coordinate.
	 * @param InZ Z Coordinate.
	 */
	inline FVector(float InX, float InY, float InZ);

	/**
	 * Constructor using the XYZ components from a 4D vector.
	 *
	 * @param V 4D Vector to copy from.
	 */
	inline FVector(const FVector4& V);

	/**
	 * Constructs a vector from an FIntPoint.
	 *
	 * @param A Int Point used to set X and Y coordinates, Z is set to zero.
	 */
	explicit FVector(FIntPoint A);

	/**
	 * Constructor which initializes all components to zero.
	 *
	 * @param EForceInit Force init enum
	 */
	explicit inline FVector(EForceInit);

	/**
	 * Gets the result of component-wise subtraction of this by another vector.
	 *
	 * @param V The vector to subtract from this.
	 * @return The result of vector subtraction.
	 */
	inline FVector operator-(const FVector& V) const;

	/**
	 * Get a negated copy of the vector.
	 *
	 * @return A negated copy of the vector.
	 */
	inline FVector operator-() const;
};


FVector::FVector()
{}

inline FVector::FVector(float InF)
	: X(InF), Y(InF), Z(InF)
{
	//DiagnosticCheckNaN();
}

FVector::FVector(float InX, float InY, float InZ)
	: X(InX), Y(InY), Z(InZ)
{
	//DiagnosticCheckNaN();
}

inline FVector::FVector(FIntPoint A)
	: X(A.X), Y(A.Y), Z(0.f)
{
	//DiagnosticCheckNaN();
}


inline FVector::FVector(EForceInit)
	: X(0.0f), Y(0.0f), Z(0.0f)
{
	//DiagnosticCheckNaN();
}

FVector FVector::operator-(const FVector& V) const
{
	return FVector(X - V.X, Y - V.Y, Z - V.Z);
}

FVector FVector::operator-() const
{
	return FVector(-X, -Y, -Z);
}

#endif
