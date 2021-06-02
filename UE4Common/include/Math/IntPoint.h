#ifndef INT_POINT_H
#define INT_POINT_H
#include "Misc/CoreMiscDefines.h"


/**
 * Structure for integer points in 2-d space.
 *
 * @todo Docs: The operators need better documentation, i.e. what does it mean to divide a point?
 */
struct FIntPoint
{
	/** Holds the point's x-coordinate. */
	int X;
	
	/** Holds the point's y-coordinate. */
	int Y;

public:

	/** Default constructor (no initialization). */
	FIntPoint();

	/**
	 * Create and initialize a new instance with the specified coordinates.
	 *
	 * @param InX The x-coordinate.
	 * @param InY The y-coordinate.
	 */
	FIntPoint(int32 InX, int32 InY);

	/**
	 * Create and initialize a new instance to zero.
	 *
	 * @param EForceInit Force init enum
	 */
	explicit FIntPoint(EForceInit);

    
};


FIntPoint::FIntPoint() { }


FIntPoint::FIntPoint(int32 InX, int32 InY)
	: X(InX)
	, Y(InY)
{ }


FIntPoint::FIntPoint(EForceInit)
	: X(0)
	, Y(0)
{ }


#endif