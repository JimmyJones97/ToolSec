#ifndef VECTOR_H
#define VECTOR_H
#include "Vector4.h"


struct FVector : public FVector
{
public:
    float X;
    float Y;
    float Z;

public:

	/** Default constructor (no initialization). */
	FVector();    

	/**
	 * Constructor using initial values for each component.
	 *
	 * @param InX X Coordinate.
	 * @param InY Y Coordinate.
	 * @param InZ Z Coordinate.
	 */
	FVector(float InX, float InY, float InZ);

	/**
	 * Constructor using the XYZ components from a 4D vector.
	 *
	 * @param V 4D Vector to copy from.
	 */
	FVector(const FVector4& V);

	/**
	 * Gets the result of component-wise subtraction of this by another vector.
	 *
	 * @param V The vector to subtract from this.
	 * @return The result of vector subtraction.
	 */
	FVector operator-(const FVector& V) const;

	/**
	 * Get a negated copy of the vector.
	 *
	 * @return A negated copy of the vector.
	 */
	FVector operator-() const;
};


FVector::FVector()
{}

FVector::FVector(float InX, float InY, float InZ)
	: X(InX), Y(InY), Z(InZ)
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


FVector::FVector( const FVector4& V )
	: X(V.X), Y(V.Y), Z(V.Z)
{
	//DiagnosticCheckNaN();
}

#endif
