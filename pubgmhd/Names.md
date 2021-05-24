# TStaticIndirectArrayThreadSafeRead
```cpp
/**
 * Simple array type that can be expanded without invalidating existing entries.
 * This is critical to thread safe FNames.
 * @param ElementType Type of the pointer we are storing in the array
 * @param MaxTotalElements absolute maximum number of elements this array can ever hold
 * @param ElementsPerChunk how many elements to allocate in a chunk
 **/
 template<typename ElementType, int32 MaxTotalElements, int32 ElementsPerChunk>
class TStaticIndirectArrayThreadSafeRead
{
	enum
	{
		// figure out how many elements we need in the master table
		ChunkTableSize = (MaxTotalElements + ElementsPerChunk - 1) / ElementsPerChunk
	};
	/** Static master table to chunks of pointers **/
	ElementType** Chunks[ChunkTableSize];
	/** Number of elements we currently have **/
	int32 NumElements;
	/** Number of chunks we currently have **/
	int32 NumChunks;

    // ...
}
```

对于下文中的`TNameEntryArray`来说，`ChunkTableSize = 128`。


# TNameEntryArray
```cpp
// Typedef for the threadsafe master name table. 
// CAUTION: If you change those constants, you probably need to update the debug visualizers.
typedef TStaticIndirectArrayThreadSafeRead<
    FNameEntry, 
    2 * 1024 * 1024 /* 2M unique FNames */, 
    16384 /* allocated in 64K/128K chunks */ > TNameEntryArray;
```
`TNameEntryArray`是一个元素为`FNameEntry`类型的列表，总的元素个数为`2 * 1024 * 1024 = 2097152`个，每个chunk装`16384`个，因此有`2097152/16384 = 128`个chunk。

即
```cpp
FNameEntry** Chunks[128];
FNameEntry* Chunk[16384];
```

# FNameEntry
```cpp
/*----------------------------------------------------------------------------
	FNameEntry.
----------------------------------------------------------------------------*/

/** 
 * Mask for index bit used to determine whether string is encoded as TCHAR or ANSICHAR. We don't
 * add an extra bool in order to keep the name size to a minimum and 2 billion names is impractical
 * so there are a few bits left in the index.
 */
#define NAME_WIDE_MASK 0x1
#define NAME_INDEX_SHIFT 1

/**
 * A global name, as stored in the global name table.
 */
struct FNameEntry
{
private:
	/** Index of name in hash. */
	NAME_INDEX		Index;

public:
	/** Pointer to the next entry in this hash bin's linked list. */
	FNameEntry*		HashNext;

protected:
	/** Name, variable-sized - note that AllocateNameEntry only allocates memory as needed. */
	union
	{
		ANSICHAR	AnsiName[NAME_SIZE];
		WIDECHAR	WideName[NAME_SIZE];
	};

// ...

}
```

# GetItemPtr
```cpp
	/**
	 * Return a pointer to the pointer to a given element
	 * @param Index The Index of an element we want to retrieve the pointer-to-pointer for
	 **/
	FORCEINLINE_DEBUGGABLE ElementType const* const* GetItemPtr(int32 Index) const
	{
		int32 ChunkIndex = Index / ElementsPerChunk;
		int32 WithinChunkIndex = Index % ElementsPerChunk;
		checkf(IsValidIndex(Index), TEXT("IsValidIndex(%d)"), Index);
		checkf(ChunkIndex < NumChunks, TEXT("ChunkIndex (%d) < NumChunks (%d)"), ChunkIndex, NumChunks);
		checkf(Index < MaxTotalElements, TEXT("Index (%d) < MaxTotalElements (%d)"), Index, MaxTotalElements);
		ElementType** Chunk = Chunks[ChunkIndex];
		check(Chunk);
		return Chunk + WithinChunkIndex;
	}
```