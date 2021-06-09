#ifndef GENERIC_PLATFORM_MATH_H
#define GENERIC_PLATFORM_MATH_H
#include <math.h>

/**
 * Generic implementation for most platforms
 */
struct FGenericPlatformMath
{
    static float Sin( float Value ) { return sinf(Value); }
    static float Cos( float Value ) { return cosf(Value); }
	static float Tan( float Value ) { return tanf(Value); }
    /** Returns higher value in a generic way */
	template< class T > 
	static T Max( const T A, const T B )
	{
		return (A>=B) ? A : B;
	}
};
#endif