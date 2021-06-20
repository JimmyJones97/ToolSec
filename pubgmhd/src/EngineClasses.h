#ifndef ENGINE_CLASSES_H
#define ENGINE_CLASSES_H
#include <inttypes.h>
#include <wchar.h>

typedef int32_t             int32;
typedef uint8_t             uint8;
typedef uint16_t            uint16;
typedef uint32_t            uint32;
typedef uint64_t            uint64;

typedef wchar_t             WIDECHAR;
//typedef unsigned int        WIDECHAR;

/** Maximum size of name. */
enum {NAME_SIZE	= 1024};

#define NAME_WIDE_MASK 0x1
#define NAME_INDEX_SHIFT 1
typedef struct FNameEntry{
    int32_t Index;
    struct FNameEntry *HashNext;
    union{
        char AnsiName[NAME_SIZE];
        wchar_t WideName[NAME_SIZE];
    };
    
}FNameEntry;

struct TNameEntryArray{
  FNameEntry** Chunks[128];  // 000
  int32_t NumElements;        // 512
  int32_t NumChunks;         // 516
  // total: 520 = 0x208
};

typedef struct FName
{
	int32 ComparisonIndex;
	int32 Number;
}FName;


template<class T>
struct TArray
{
	friend struct FString;
public:
	inline TArray()
	{
		Data = NULL;
		Count = Max = 0;
	};
	inline int Num() const
	{
		return Count;
	};
	inline T& operator[](int i)
	{
		return Data[i];
	};
	inline const T& operator[](int i) const
	{
		return Data[i];
	};
	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}
public:
	T* Data;
	int32_t Count;
	int32_t Max;
};

class UClass;
class UObject;

class UObjectBase
{
public:
    void *VTableObject;
    /** Flags used to track and report various object states. This needs to be 8 byte aligned on 32-bit
	    platforms to reduce memory waste */
	int32 ObjectFlags; //EObjectFlags ObjectFlags;
    
	/** Index into GObjectArray...very private. */
	int32 InternalIndex;

	/** Class the object belongs to. */
	UClass *ClassPrivate; //UClass* ClassPrivate;

	/** Name of this object */
	FName NamePrivate;

	/** Object this object resides in. */
	UObject *OuterPrivate; //UObject *OuterPrivate;
    
    //void *TStatIdPtr; // 需要剔除，否则偏移不对
};
typedef struct FUObjectItem
{
	// Pointer to the allocated object
	struct UObjectBase* Object;
	
    // Internal flags
	int32 Flags;
	// UObject Owner Cluster Index
	int32 ClusterRootIndex;	
	// Weak Object Pointer Serial number associated with the object
	int32 SerialNumber;
    
	bool IsPendingKill() const
	{
		return !!(Flags & int32(1 << 29));
	}
}FUobjectItem;

typedef struct FFixedUObjectArray
{
	/** Static master table to chunks of pointers **/
	struct FUObjectItem* Objects;
    /** pubgmhd中未知字段，引擎代码中不存在 **/
    int32 unk; 
	/** Number of elements we currently have **/
	int32 MaxElements;
	/** Current number of UObject slots */
    int32 NumElements;
}FFixedUObjectArray;

typedef FFixedUObjectArray TUObjectArray;


class UObjectBaseUtility: public UObjectBase {
    
};

class UObject: public UObjectBaseUtility{
/*
public:
    void *VTable;                                                   // 0x0000 (0x4)
    int32_t ObjectFlags;                                            // 0x0004 (0x4)
    int32_t InternalIndex;                                          // 0x0008 (0x4)
    UClass *ClassPrivate;                                           // 0x000C (0x4)
    FName NamePrivate;                                              // 0x0010 (0x8)
    UObject *Outer;                                                 // 0x0018 (0x4)
    void *TStatIdPtr;                                               // 0x001C (0x4)
    // 0x0020 UObject ends 
*/
};



class UField: public UObject{
public:
    UField *Next;                                                   // 0x0020 (0x4)
    // 0x0024 UField ends
};

class UProperty: public UField{
public:
    int32_t ArrayDim;                                               // 0x0024 (0x4)
    int32_t ElementSize;                                            // 0x0028 (0x4)
    uint64_t PropertyFlags;                                         // 0x002C (0x8)
    uint16_t RepIndex;                                              // 0x0034 (0x2)
    char dummy_BlueprintReplicationCondition;                       // 0x0036 (0x1)
    char pad_0x0037[0x1];                                           // 0x0037 (0x1) // align to 4
    int32_t Offset_Internal;                                        // 0x0038 (0x4)
    FName RepNotifyFunc;                                            // 0x003C (0x8)
    UProperty *PropertyLinkNext;                                    // 0x0040 (0x4)
    UProperty *NextRef;                                             // 0x0044 (0x4)
    UProperty *DestructorLinkNext;                                  // 0x0048 (0x4)
    UProperty *PostConstructLinkNext;                               // 0x004C (0x4)
    // 0x0050 UProperty ends
};

class UStruct: public UField{
public:
    UStruct *SuperStruct;                                           // 0x0024 (0x4)
    UField *Children;                                               // 0x0028 (0x4)
    int32_t PropertiesSize;                                         // 0x002c (0x4)
    int32_t MinAlignment;                                           // 0x0030 (0x4)
    char Script[0xC];                                               // 0x0034 (0xC) // 是一个TArray，在Android里12字节
    UProperty *PropertyLink;                                        // 0x0040 (0x4)
    UProperty* RefLink;                                             // 0x0044 (0x4)
    UProperty* DestructorLink;                                      // 0x0048 (0x4)
    UProperty* PostConstructLink;                                   // 0x004c (0x4)
    char ScriptObjectReferences[0xC];                               // 0x0050 (0xC) // 是一个TArray，在Android里12字节
    // 0x005c UStruct ends // Note: 刺激战场里错误，从log日志看应该是0x58，可能这里被修改过
};

class UScriptStruct : public UStruct
{
public:
	char pad[0x10];
};

class UStructProperty : public UProperty
{
public:
	UScriptStruct* Struct;
};

class UObjectPropertyBase : public UProperty
{
public:
	UClass* PropertyClass;
};

class UClass: public UStruct{
public:
    char dummy[0x100]; // FIXME: not know actually size....
};

/*
[00005468]    Function                PrestreamTextures                                     0xbc62b120    outerName: Actor
	FunctionFlags:           0xbc62b178    67240960 // 偏移是0x58
	NumParms:                0xbc62b17c    3
	ParmsSize:               0xbc62b17e    12
	ReturnValueOffset:       0xbc62b180    65535
	RPCId:                   0xbc62b182    0
	RPCResponseId:           0xbc62b184    0
	FirstPropertyToInit:     0xbc62b188    00000000
	EventGraphFunction:      0xbc62b18c    00000000
	EventGraphCallOffset:    0xbc62b190    0
	Func:                    0xbc62b194    0xc662ffdc (UE4:0xc4134000, offset:0x24fbfdc)
*/

class UFunction: public UStruct{
public:
    int FunctionFlags;                                              // 0x58 // 起始0x58的原因，注意看UStruct
    uint8 NumParms;                                                 // 0x5C (align: +1)
    uint16 ParmsSize;                                               // 0x5E
    uint16 ReturnValueOffset;                                       // 0x60
    uint16 RPCId;                                                   // 0x62
    uint16 RPCResponseId;                                           // 0x64
    UProperty* FirstPropertyToInit;                                 // 0x68 (align: +2)
    void *EventGraphFunction;                                       // 0x6c
    int32 EventGraphCallOffset;                                     // 0x60
    void *Func;                                                     // 0x74 Native function pointer 
    // v11 = *(int (**)(void))(v3 + 0x74); v11 实际是Native Function的地址
    // 0x78 UFunction ends
};

class TARRAY{
public:
    unsigned int AllocatorInstance; // 其实是一个指针，指向存储的队列数据
    int32 ArrayNum;
    int32 ArrayMax;
    
};

class UMulticastDelegateProperty: public UProperty
{
public:
    UFunction* SignatureFunction;
};

class UObjectProperty: public UProperty{
public:
    UClass* PropertyClass;
};

class UArrayProperty: public UProperty{
public:
    UProperty* Inner;
};


// 与UE4Common中重复定义，先注释掉
// class FVector 
// {
// public:

// 	/** Vector's X component. */
// 	float X;

// 	/** Vector's Y component. */
// 	float Y;

// 	/** Vector's Z component. */
// 	float Z;
// };



#endif