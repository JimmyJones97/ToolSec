#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/uio.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <map>

typedef int int32;
typedef unsigned char       uint8;
typedef unsigned short int  uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

/** Maximum size of name. */
enum {NAME_SIZE	= 1024};

unsigned int G_OFF_NAMES = 0x6E6B164;
unsigned int G_ADDR_NAMES = 0; //also dword_6E6B164
pid_t G_PID = -1;

unsigned int G_OFf_GUObjectArray__ObjObjects = 0x6EF2F60;
unsigned int G_ADDR_GUObjectArray__ObjObjects = 0;

int find_pid_of(const char *process_name)  
{  
    int id;  
    pid_t pid = -1;  
    DIR* dir;  
    FILE *fp;  
    char filename[32];  
    char cmdline[256];  
  
    struct dirent * entry;  
  
    if (process_name == NULL)  
        return -1;  
  
    dir = opendir("/proc");  
    if (dir == NULL)  
        return -1;  
  
    while((entry = readdir(dir)) != NULL) {  
        id = atoi(entry->d_name);  
        if (id != 0) {  
            sprintf(filename, "/proc/%d/cmdline", id);  
            fp = fopen(filename, "r");  
            if (fp) {  
                fgets(cmdline, sizeof(cmdline), fp);  
                fclose(fp);  
  
                if (strcmp(process_name, cmdline) == 0) {  
                    /* process found */  
                    pid = id;  
                    break;  
                }  
            }  
        }  
    }  
  
    closedir(dir);  
    return pid;  
}  


unsigned long get_module_base(pid_t pid, const char* module_name)
{
    FILE *fp;
    unsigned long addr = 0;
    char *pch;
    char filename[32];
    char line[1024];

    snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);

    fp = fopen(filename, "r");

    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, module_name) && strstr(line, "r-xp")) {
                pch = strtok(line, "-");
                addr = strtoul(pch, NULL, 16);
                if (addr == 0x8000)
                    addr = 0;
                break;
            }
        }
        fclose(fp);
    }
    return addr;
}


int readNBytes(void *rbase, void *lbase, unsigned int n){
        int retval = -1;

        struct iovec local[1];
        local[0].iov_base = lbase;
        local[0].iov_len = n;

        struct iovec remote[1];
        remote[0].iov_base = rbase;
        remote[0].iov_len = n;
        //printf("[!] prepare to process_vm_readv %d (%lx) == %x\n",child,  (unsigned long)remote[0].iov_base, remote[0].iov_len);
        //retval = process_vm_readv(child, local, 1, remote, 1, 0);

        //retval = syscall(270, G_PID, local, 1, remote, 1, 0);

        retval = syscall(376, G_PID, local, 1, remote, 1, 0);

        //printf("[!] end to process_vm_readv %x %d\n",  retval,errno);
        if (retval < 0) {
            printf("[!] failed to process_vm_readv(%d) == %d, %s\n", retval, errno, strerror(errno));
            return -1;
        }
        return 0; // read success    
}

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

static struct TNameEntryArray *Names = NULL;


typedef struct FName
{
	int32 ComparisonIndex;
	int32 Number;
}FName;


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

class FVector 
{
public:

	/** Vector's X component. */
	float X;

	/** Vector's Y component. */
	float Y;

	/** Vector's Z component. */
	float Z;
};


char* GetNameByIndex(int Index, char *ret)
{
    
    if(!G_ADDR_NAMES){
        printf("error: G_ADDR_NAMES is NULL\n");
        return NULL;
    }

    if(0 != readNBytes((void*)G_ADDR_NAMES, &Names, 4)){
        printf("error: access remote failed:0x%x\n", G_ADDR_NAMES);
        return NULL;
    }
    //printf("[dword_6E6B164] = [0x%08x] = 0x%08x = Names\n", G_ADDR_NAMES, (unsigned long)Names);    

    struct TNameEntryArray name_entry_array = { 0 };
    if(0 != readNBytes((void*)Names, (void*)&name_entry_array, sizeof(struct TNameEntryArray))){
        printf("error: access remote failed:0x%08x\n", (unsigned long)Names);
        return NULL;
    }
    Names = (TNameEntryArray *)&name_entry_array;
    //printf("Names->NumElements=%d, Names->NumChunks=%d, Names->Chunks=%p\n", \
                Names->NumElements, Names->NumChunks, Names->Chunks);
    // for(int i=0; i<128; i++){
    //     printf("Chunks[%d]=%p\n", i, Names->Chunks[i]);
    // }

    int ChunkIndex = Index / 0x4000; // 0x4000 = 16384, 每个Chunk容量16384个FNameEntry*
    int WithinChunkIndex = Index % 0x4000;
    //printf("Index=%d, ChunkIndex=%d, WithinChunkIndex=%d\n", Index, ChunkIndex, WithinChunkIndex);

    if(Index >= Names->NumElements || Index < 0){
        printf("Index error: %d\n", Index);
        return NULL;
    }

    if(ChunkIndex>=Names->NumChunks || ChunkIndex<0){
        printf("ChunkIndex error: %d\n", ChunkIndex);
        return NULL;
    }

    FNameEntry** Chunk = Names->Chunks[ChunkIndex];
    FNameEntry** ItemPtr = Chunk + WithinChunkIndex;

    //printf("Chunk=0x%08x, ItemPtr=0x%08x\n", (unsigned long)Chunk, (unsigned long)ItemPtr);

    unsigned int addr_name_entry = 0;
    if(0 != readNBytes((void*)ItemPtr, (void*)&addr_name_entry, 4)){
        printf("error: access remote failed:0x%08x\n", (unsigned long)ItemPtr);
        return NULL;        
    }
    //printf("addr_name_entry=0x%08x\n", addr_name_entry);
    if(!addr_name_entry)return NULL;

    FNameEntry name_entry = { 0 };
    if(0 != readNBytes((void*)addr_name_entry, (void*)&name_entry, sizeof(FNameEntry))){
        printf("error: access remote failed:0x%08x\n", addr_name_entry);
        return NULL;        
    }
    int isWide = name_entry.Index & NAME_WIDE_MASK;
    //printf("FNameEntry Index=%d, HashNext=0x%08x, isWide=%d, sizeof(FNameEntry)=%d\n", \
            name_entry.Index, name_entry.HashNext, isWide, sizeof(FNameEntry));
    //printf(">>>>%s\n", name_entry.AnsiName);
    strcpy(ret, name_entry.AnsiName);
    //printf("Index=%d, AnsiName:%s\n", Index, ret);    

    // 打印所有
    // for(int k=0; k<Names->NumElements; k++){
    //     do{
    //         printf("---------------------------\n");
    //         Index = k;
    //         int ChunkIndex = Index / 0x4000; // 0x4000 = 16384, 每个Chunk容量16384个FNameEntry*
    //         int WithinChunkIndex = Index % 0x4000;
    //         printf("Index=%d, ChunkIndex=%d, WithinChunkIndex=%d\n", Index, ChunkIndex, WithinChunkIndex);

    //         if(Index >= Names->NumElements || Index < 0){
    //             printf("Index error: %d\n", Index);
    //             return NULL;
    //         }

    //         if(ChunkIndex>=Names->NumChunks || ChunkIndex<0){
    //             printf("ChunkIndex error: %d\n", ChunkIndex);
    //             return NULL;
    //         }

    //         FNameEntry** Chunk = Names->Chunks[ChunkIndex];
    //         FNameEntry** ItemPtr = Chunk + WithinChunkIndex;

    //         printf("Chunk=0x%08x, ItemPtr=0x%08x\n", (unsigned long)Chunk, (unsigned long)ItemPtr);

    //         unsigned int addr_name_entry = 0;
    //         if(0 != readNBytes((void*)ItemPtr, (void*)&addr_name_entry, 4)){
    //             printf("error: access remote failed:0x%08x\n", (unsigned long)ItemPtr);
    //             return NULL;        
    //         }
    //         printf("addr_name_entry=0x%08x\n", addr_name_entry);
    //         if(!addr_name_entry)break;

    //         FNameEntry name_entry = { 0 };
    //         if(0 != readNBytes((void*)addr_name_entry, (void*)&name_entry, sizeof(FNameEntry))){
    //             printf("error: access remote failed:0x%08x\n", addr_name_entry);
    //             return NULL;        
    //         }
    //         int isWide = name_entry.Index & NAME_WIDE_MASK;
    //         printf("FNameEntry Index=%d, HashNext=0x%08x, isWide=%d, sizeof(FNameEntry)=%d\n", 
    //                 name_entry.Index, name_entry.HashNext, isWide, sizeof(FNameEntry));
    //         printf(">>>>%s\n", name_entry.AnsiName);
    //     }while(0);
    // }


    return ret;
}


static TUObjectArray *pGUObjectArray__ObjObjects = NULL;
void readObjects(){
    // &GUObjectArray.ObjObjects;
    if(!G_ADDR_GUObjectArray__ObjObjects){
        printf("error: G_ADDR_GUObjectArray__ObjObjects is NULL\n");
        return;
    }
    FFixedUObjectArray fixed_uobject_array = { 0 };
    // 用于发现被修改后的FFixedUObjectArray结构
    // unsigned int temp [10];
    // if(0 != readNBytes((void*)G_ADDR_GUObjectArray__ObjObjects, (void*)&temp, sizeof(temp))){
    //     printf("error: access temp\n");
    //     return;
    // }
    // for(int i=0; i<10; i++){
    //     printf("[%d]0x%08x\n", i, temp[i]);
    // }
    if(0 != readNBytes((void*)G_ADDR_GUObjectArray__ObjObjects, (void*)&fixed_uobject_array, sizeof(FFixedUObjectArray))){
        printf("error: access remote failed:0x%08x\n", G_ADDR_GUObjectArray__ObjObjects);
        return;
    }
    // MaxElements, NumElements看着不对，可能是偏移有问题。
    printf("Objects=0x%08x, MaxElements=%d, NumElements=%d\n", \
            fixed_uobject_array.Objects, fixed_uobject_array.MaxElements, fixed_uobject_array.NumElements);
    
    for(int i=0; i<fixed_uobject_array.NumElements; i++){
        FUObjectItem item = { 0 };
        if(0 != readNBytes((void*)(fixed_uobject_array.Objects+i), (void*)&item, sizeof(FUObjectItem))){
            printf("error: access remote failed:0x%08x\n", fixed_uobject_array.Objects);
            return;
        }
        printf("[%d] Object=0x%08x, Flags=0x%08x, ClusterRootIndex=%d, SerialNumber=%d\n", \
                i, item.Object, item.Flags, item.ClusterRootIndex, item.SerialNumber);

        if(!item.Object)continue;
        
        UObjectBase obj = { 0 };
        if(0 != readNBytes((void*)(item.Object), (void*)&obj, sizeof(UObjectBase))){
            printf("error: access remote failed, Objects[%d].Object is 0x%08x\n", i, item.Object);
            return;        
        }
        UObjectBase objClass = { 0 };
        if(0 != readNBytes((void*)(obj.ClassPrivate), (void*)&objClass, sizeof(UObjectBase))){
            printf("error: access remote failed, objClass:0x%08x\n", obj.ClassPrivate);
            return;        
        }
        UObjectBase objOuter = { 0 };
        if(obj.OuterPrivate){
            if(0 != readNBytes((void*)(obj.OuterPrivate), (void*)&objOuter, sizeof(UObjectBase))){
                printf("error: access remote failed, objOuter:0x%08x\n", obj.OuterPrivate);
                return;        
            }            
        }
        

        char objName[256] = { 0 };
        char objClassName[256] = { 0 };
        char objOuterName[256] = { 0 };

        printf("    VTableObject=0x%08x, ObjectFlags=0x%08x, InternalIndex=%d, ClassPrivate=0x%08x(%s), NamePrivate.ComparisonIndex=%d(%s), NamePrivate.Number=%d, OuterPrivate=0x%08x(%s)\n",\
                obj.VTableObject, 
                obj.ObjectFlags, 
                obj.InternalIndex, 
                obj.ClassPrivate,
                GetNameByIndex(objClass.NamePrivate.ComparisonIndex, objClassName), 
                obj.NamePrivate.ComparisonIndex, 
                GetNameByIndex(obj.NamePrivate.ComparisonIndex, objName), 
                obj.NamePrivate.Number, 
                obj.OuterPrivate,
                obj.OuterPrivate? GetNameByIndex(objOuter.NamePrivate.ComparisonIndex, objOuterName) : NULL);

    }

    // void *VTableObject;
    // /** Flags used to track and report various object states. This needs to be 8 byte aligned on 32-bit
	//     platforms to reduce memory waste */
	// int32 ObjectFlags; //EObjectFlags ObjectFlags;
    
	// /** Index into GObjectArray...very private. */
	// int32 InternalIndex;

	// /** Class the object belongs to. */
	// UClass *ClassPrivate; //UClass* ClassPrivate;

	// /** Name of this object */
	// FName NamePrivate;

	// /** Object this object resides in. */
	// UObject *OuterPrivate; //UObject *OuterPrivate;
    
    // //void *TStatIdPtr; // 需要剔除，否则偏移不对

// typedef struct FName
// {
// 	int32 ComparisonIndex;
// 	int32 Number;
// }FName;

}

static std::map<unsigned int, unsigned int> STCharacterMovementComponent_map;
void readLocationByMovementComp(){
    // &GUObjectArray.ObjObjects;
    if(!G_ADDR_GUObjectArray__ObjObjects){
        printf("error: G_ADDR_GUObjectArray__ObjObjects is NULL\n");
        return;
    }
    FFixedUObjectArray fixed_uobject_array = { 0 };
    // 用于发现被修改后的FFixedUObjectArray结构
    // unsigned int temp [10];
    // if(0 != readNBytes((void*)G_ADDR_GUObjectArray__ObjObjects, (void*)&temp, sizeof(temp))){
    //     printf("error: access temp\n");
    //     return;
    // }
    // for(int i=0; i<10; i++){
    //     printf("[%d]0x%08x\n", i, temp[i]);
    // }
    if(0 != readNBytes((void*)G_ADDR_GUObjectArray__ObjObjects, (void*)&fixed_uobject_array, sizeof(FFixedUObjectArray))){
        printf("error: access remote failed:0x%08x\n", G_ADDR_GUObjectArray__ObjObjects);
        return;
    }
    // MaxElements, NumElements看着不对，可能是偏移有问题。
    // printf("Objects=0x%08x, MaxElements=%d, NumElements=%d\n", \
    //         fixed_uobject_array.Objects, fixed_uobject_array.MaxElements, fixed_uobject_array.NumElements);
    
    for(int i=0; i<fixed_uobject_array.NumElements; i++){
        FUObjectItem item = { 0 };
        if(0 != readNBytes((void*)(fixed_uobject_array.Objects+i), (void*)&item, sizeof(FUObjectItem))){
            printf("error: access remote failed:0x%08x\n", fixed_uobject_array.Objects);
            return;
        }
        // printf("[%d] Object=0x%08x, Flags=0x%08x, ClusterRootIndex=%d, SerialNumber=%d\n", \
        //         i, item.Object, item.Flags, item.ClusterRootIndex, item.SerialNumber);

        if(!item.Object)continue;
        
        UObjectBase obj = { 0 };
        if(0 != readNBytes((void*)(item.Object), (void*)&obj, sizeof(UObjectBase))){
            printf("error: access remote failed, Objects[%d].Object is 0x%08x\n", i, item.Object);
            return;        
        }
        UObjectBase objClass = { 0 };
        if(0 != readNBytes((void*)(obj.ClassPrivate), (void*)&objClass, sizeof(UObjectBase))){
            printf("error: access remote failed, objClass:0x%08x\n", obj.ClassPrivate);
            return;        
        }
        UObjectBase objOuter = { 0 };
        if(obj.OuterPrivate){
            if(0 != readNBytes((void*)(obj.OuterPrivate), (void*)&objOuter, sizeof(UObjectBase))){
                printf("error: access remote failed, objOuter:0x%08x\n", obj.OuterPrivate);
                return;        
            }            
        }
        

        char objName[256] = { 0 };
        char objClassName[256] = { 0 };
        char objOuterName[256] = { 0 };

        // printf("    VTableObject=0x%08x, ObjectFlags=0x%08x, InternalIndex=%d, ClassPrivate=0x%08x(%s), NamePrivate.ComparisonIndex=%d(%s), NamePrivate.Number=%d, OuterPrivate=0x%08x(%s)\n",\
        //         obj.VTableObject, 
        //         obj.ObjectFlags, 
        //         obj.InternalIndex, 
        //         obj.ClassPrivate,
        //         GetNameByIndex(objClass.NamePrivate.ComparisonIndex, objClassName), 
        //         obj.NamePrivate.ComparisonIndex, 
        //         GetNameByIndex(obj.NamePrivate.ComparisonIndex, objName), 
        //         obj.NamePrivate.Number, 
        //         obj.OuterPrivate,
        //         obj.OuterPrivate? GetNameByIndex(objOuter.NamePrivate.ComparisonIndex, objOuterName) : NULL);

        if(strstr(GetNameByIndex(objClass.NamePrivate.ComparisonIndex, objClassName), "STCharacterMovementComponent") && strstr(GetNameByIndex(obj.NamePrivate.ComparisonIndex, objName), "CharMoveComp")) 
        {
            if(STCharacterMovementComponent_map.find((unsigned int)item.Object) == STCharacterMovementComponent_map.end()){
                printf("find a new CharMoveComp:STCharacterMovementComponent: 0x%08x\n", (unsigned int)item.Object);
                STCharacterMovementComponent_map[(unsigned int)item.Object] = (unsigned int)item.Object;
            }
        }
    }

    while(true){
        std::map<unsigned int, unsigned int>::iterator it;
        for(it=STCharacterMovementComponent_map.begin(); it != STCharacterMovementComponent_map.end(); ++it){
            unsigned int p_CharMoveComp = it->first;
            FVector LastUpdateLocation;
            if(0 != readNBytes((void*)(p_CharMoveComp + 0x27c), (void*)&LastUpdateLocation, sizeof(FVector))){
                printf("error: access remote LastUpdateLocation failed:0x%08x, size:%d\n",p_CharMoveComp + 0x27c, sizeof(FVector) );
                continue;        
            }
            printf("p_CharMoveComp=0x%08x, LastUpdateLocation=(%f,%f,%f)\n", p_CharMoveComp, LastUpdateLocation.X, LastUpdateLocation.Y, LastUpdateLocation.Z);   
        }
        printf("-----\n");

        sleep(5);
    }    
}

int main(int argc, char *argv[]){
    // Usage: readname <index>
    G_PID = find_pid_of("com.tencent.tmgp.pubgmhd");
    if(-1 == G_PID){
        printf("cannot find pid of com.tencent.tmgp.pubgmhd\n");
        return -1;
    }
    printf("pid=%d\n", G_PID);
    unsigned int libUE4_base = get_module_base(G_PID, "libUE4.so");
    if(0 == libUE4_base){
        printf("error: libUE4 base is NULL\n");
        return -1;
    }
    
    G_ADDR_NAMES = libUE4_base + G_OFF_NAMES;
    printf("libUE4.so base=0x%08x, G_ADDR_NAMES = 0x%08x + 0x%08x = 0x%08x\n", libUE4_base, libUE4_base, G_OFF_NAMES, G_ADDR_NAMES);
    Names = (struct TNameEntryArray*)G_ADDR_NAMES;

    G_ADDR_GUObjectArray__ObjObjects = libUE4_base + G_OFf_GUObjectArray__ObjObjects;
    printf("libUE4.so base=0x%08x, G_ADDR_GUObjectArray__ObjObjects = 0x%08x + 0x%08x = 0x%08x\n", \
            libUE4_base, libUE4_base, G_OFf_GUObjectArray__ObjObjects, G_ADDR_GUObjectArray__ObjObjects);
    
    char s[1024] = { 0 };
    //GetNameByIndex(1199, s);
    //readObjects();
    readLocationByMovementComp();

    return 0;
}