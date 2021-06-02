#ifndef VECTOR4_H
#define VECTOR4_H


/**
 * A 4D homogeneous vector, 4x1 FLOATs, 16-byte aligned.
 */
struct FVector4
{
public:

	/** The vector's X-component. */
	float X;

	/** The vector's Y-component. */
	float Y;

	/** The vector's Z-component. */
	float Z;

	/** The vector's W-component. */
	float W;

public:
	/**
	 * Constructor.
	 *
	 * @param InVector 3D Vector to set first three components.
	 * @param InW W Coordinate.
	 */
	FVector4(const FVector& InVector, float InW = 1.0f);

	/**
	 * Creates and initializes a new vector from the specified components.
	 *
	 * @param InX X Coordinate.
	 * @param InY Y Coordinate.
	 * @param InZ Z Coordinate.
	 * @param InW W Coordinate.
	 */
	explicit FVector4(float InX = 0.0f, float InY = 0.0f, float InZ = 0.0f, float InW = 1.0f);
};


/* FVector4 inline functions
 *****************************************************************************/

FVector4::FVector4(const FVector& InVector,float InW)
	: X(InVector.X)
	, Y(InVector.Y)
	, Z(InVector.Z)
	, W(InW)
{
	//DiagnosticCheckNaN();
}


#endif