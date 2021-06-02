#ifndef GENERIC_PLATFORM_MEMORY_H
#define GENERIC_PLATFORM_MEMORY_H
#include <string.h>

typedef unsigned long SIZE_T;

/** Generic implementation for most platforms, these tend to be unused and unimplemented. */
struct FGenericPlatformMemory
{

	static void* Memcpy(void* Dest, const void* Src, SIZE_T Count)
	{
		return memcpy( Dest, Src, Count );
	}
};

#endif