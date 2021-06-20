#ifndef VECTOR2D_H
#define VECTOR2D_H
#include "../Misc/CoreMiscDefines.h"
#include "../Math/IntPoint.h"
/**
 * A vector in 2-D space composed of components (X, Y) with floating point precision.
 */
struct FVector2D 
{
	/** Vector's X component. */
	float X;

	/** Vector's Y component. */
	float Y;

public:

	/** Default constructor (no initialization). */
	inline FVector2D() { }

	/**
	 * Constructor using initial values for each component.
	 *
	 * @param InX X coordinate.
	 * @param InY Y coordinate.
	 */
	inline FVector2D(float InX, float InY);

	/**
	 * Constructs a vector from an FIntPoint.
	 *
	 * @param InPos Integer point used to set this vector.
	 */
	inline FVector2D(FIntPoint InPos);

	/**
	 * Constructor which initializes all components to zero.
	 *
	 * @param EForceInit Force init enum
	 */
	inline explicit FVector2D(EForceInit);


public:

	/**
	 * Gets the result of adding two vectors together.
	 *
	 * @param V The other vector to add to this.
	 * @return The result of adding the vectors together.
	 */
	inline FVector2D operator+(const FVector2D& V) const;

	/**
	 * Gets the result of subtracting a vector from this one.
	 *
	 * @param V The other vector to subtract from this.
	 * @return The result of the subtraction.
	 */
	inline FVector2D operator-(const FVector2D& V) const;

	/**
	 * Gets the result of scaling the vector (multiplying each component by a value).
	 *
	 * @param Scale How much to scale the vector by.
	 * @return The result of scaling this vector.
	 */
	inline FVector2D operator*(float Scale) const;

	/**
	 * Gets the result of dividing each component of the vector by a value.
	 *
	 * @param Scale How much to divide the vector by.
	 * @return The result of division on this vector.
	 */
	inline FVector2D operator/(float Scale) const;

	/**
	 * Gets the result of this vector + float A.
	 *
	 * @param A Float to add to each component.
	 * @return The result of this vector + float A.
	 */
	inline FVector2D operator+(float A) const;

	/**
	 * Gets the result of subtracting from each component of the vector.
	 *
	 * @param A Float to subtract from each component
	 * @return The result of this vector - float A.
	 */
	inline FVector2D operator-(float A) const;

	/**
	 * Gets the result of component-wise multiplication of this vector by another.
	 *
	 * @param V The other vector to multiply this by.
	 * @return The result of the multiplication.
	 */
	inline FVector2D operator*(const FVector2D& V) const;

	/**
	 * Gets the result of component-wise division of this vector by another.
	 *
	 * @param V The other vector to divide this by.
	 * @return The result of the division.
	 */
	inline FVector2D operator/(const FVector2D& V) const;

	/**
	 * Calculates dot product of this vector and another.
	 *
	 * @param V The other vector.
	 * @return The dot product.
	 */
	inline float operator|(const FVector2D& V) const;

	/**
	 * Calculates cross product of this vector and another.
	 *
	 * @param V The other vector.
	 * @return The cross product.
	 */
	inline float operator^(const FVector2D& V) const;

	/**
	 * Checks whether all components of the vector are exactly zero.
	 *
	 * @return true if vector is exactly zero, otherwise false.
	 */
	inline bool IsZero() const;
};


FVector2D::FVector2D(float InX,float InY)
	:	X(InX), Y(InY)
{ }


FVector2D::FVector2D(FIntPoint InPos)
{
	X = (float)InPos.X;
	Y = (float)InPos.Y;
}


FVector2D::FVector2D(EForceInit)
	: X(0), Y(0)
{
}


FVector2D FVector2D::operator+(const FVector2D& V) const
{
	return FVector2D(X + V.X, Y + V.Y);
}


FVector2D FVector2D::operator-(const FVector2D& V) const
{
	return FVector2D(X - V.X, Y - V.Y);
}


FVector2D FVector2D::operator*(float Scale) const
{
	return FVector2D(X * Scale, Y * Scale);
}


FVector2D FVector2D::operator/(float Scale) const
{
	const float RScale = 1.f/Scale;
	return FVector2D(X * RScale, Y * RScale);
}


FVector2D FVector2D::operator+(float A) const
{
	return FVector2D(X + A, Y + A);
}


FVector2D FVector2D::operator-(float A) const
{
	return FVector2D(X - A, Y - A);
}


FVector2D FVector2D::operator*(const FVector2D& V) const
{
	return FVector2D(X * V.X, Y * V.Y);
}


FVector2D FVector2D::operator/(const FVector2D& V) const
{
	return FVector2D(X / V.X, Y / V.Y);
}


float FVector2D::operator|(const FVector2D& V) const
{
	return X*V.X + Y*V.Y;
}


float FVector2D::operator^(const FVector2D& V) const
{
	return X*V.Y - Y*V.X;
}

bool FVector2D::IsZero() const
{
	return X==0.f && Y==0.f;
}
#endif