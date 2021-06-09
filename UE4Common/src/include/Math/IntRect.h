#ifndef INT_RECT_H
#define INT_RECT_H
#include <stdint.h>
typedef int32_t int32;

#include "IntPoint.h"
#include "Misc/CoreMiscDefines.h"

/**
 * Structure for integer rectangles in 2-d space.
 *
 * @todo Docs: The operators need better documentation, i.e. what does it mean to divide a rectangle?
 */
struct FIntRect
{
	/** Holds the first pixel line/row (like in Win32 RECT). */
	FIntPoint Min;

	/** Holds the last pixel line/row (like in Win32 RECT). */
	FIntPoint Max;

public:

	/** Constructor */
	FIntRect();

	/**
	 * Constructor
	 *
	 * @param X0 Minimum X coordinate.
	 * @param Y0 Minimum Y coordinate.
	 * @param X1 Maximum X coordinate.
	 * @param Y1 Maximum Y coordinate.
	 */
	FIntRect( int32 X0, int32 Y0, int32 X1, int32 Y1 );

	/**
	 * Constructor
	 *
	 * @param InMin Minimum Point
	 * @param InMax Maximum Point
	 */
	FIntRect( FIntPoint InMin, FIntPoint InMax );    

public:
	/**
	 * Gets the Height of the rectangle.
	 *
	 * @return The Height of the rectangle.
	 */
	int32 Height() const;

	/**
	 * Gets the width of the rectangle.
	 *
	 * @return The width of the rectangle.
	 */
	int32 Width() const;
};


FIntRect::FIntRect()
	: Min(ForceInit)
	, Max(ForceInit)
{ }


FIntRect::FIntRect( int32 X0, int32 Y0, int32 X1, int32 Y1 )
	: Min(X0, Y0)
	, Max(X1, Y1)
{ }


FIntRect::FIntRect( FIntPoint InMin, FIntPoint InMax )
	: Min(InMin)
	, Max(InMax)
{ }

int32 FIntRect::Height() const
{
	return (Max.Y - Min.Y);
}


int32 FIntRect::Width() const
{
	return Max.X-Min.X;
}

#endif