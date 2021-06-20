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
#include <locale.h>
#include <wchar.h>

#include "EngineClasses.h"
#include "RemoteUtils.h"
#include "UE4Names.h"
#include "DumpUtils.h"

#include "../../UE4Common/src/include/Camera/CameraTypes.h"
#include "../../UE4Common/src/include/MySimulation.h"
#include "../../UE4Common/src/include/SceneView.h"

unsigned int G_OFF_NAMES = 0x6E6B164;
unsigned int G_ADDR_NAMES = 0; //also dword_6E6B164
pid_t G_PID = -1;

unsigned int G_OFf_GUObjectArray__ObjObjects = 0x6EF2F60;
unsigned int G_ADDR_GUObjectArray__ObjObjects = 0;

unsigned int G_libUE4_base = 0;

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


void readObjects(){
    // &GUObjectArray.ObjObjects;
    if(!G_ADDR_GUObjectArray__ObjObjects){
        printf("error: G_ADDR_GUObjectArray__ObjObjects is NULL\n");
        return;
    }
    FFixedUObjectArray fixed_uobject_array = { 0 };
    // 用于发现被修改后的FFixedUObjectArray结构
    // unsigned int temp [10];
    // if(0 != readNBytes(G_PID, (void*)G_ADDR_GUObjectArray__ObjObjects, (void*)&temp, sizeof(temp))){
    //     printf("error: access temp\n");
    //     return;
    // }
    // for(int i=0; i<10; i++){
    //     printf("[%d]0x%08x\n", i, temp[i]);
    // }
    if(0 != readNBytes(G_PID, (void*)G_ADDR_GUObjectArray__ObjObjects, (void*)&fixed_uobject_array, sizeof(FFixedUObjectArray))){
        printf("error: access remote failed:0x%08x\n", G_ADDR_GUObjectArray__ObjObjects);
        return;
    }
    // MaxElements, NumElements看着不对，可能是偏移有问题。
    printf("Objects=0x%08x, MaxElements=%d, NumElements=%d\n", \
            fixed_uobject_array.Objects, fixed_uobject_array.MaxElements, fixed_uobject_array.NumElements);
    
    for(int i=0; i<fixed_uobject_array.NumElements; i++){
        FUObjectItem item = { 0 };
        if(0 != readNBytes(G_PID, (void*)(fixed_uobject_array.Objects+i), (void*)&item, sizeof(FUObjectItem))){
            printf("error: access remote failed:0x%08x\n", fixed_uobject_array.Objects);
            return;
        }
        printf("[%d] Object=0x%08x, Flags=0x%08x, ClusterRootIndex=%d, SerialNumber=%d\n", \
                i, item.Object, item.Flags, item.ClusterRootIndex, item.SerialNumber);

        if(!item.Object)continue;
        
        UObjectBase obj = { 0 };
        if(0 != readNBytes(G_PID, (void*)(item.Object), (void*)&obj, sizeof(UObjectBase))){
            printf("error: access remote failed, Objects[%d].Object is 0x%08x\n", i, item.Object);
            return;        
        }
        UObjectBase objClass = { 0 };
        if(0 != readNBytes(G_PID, (void*)(obj.ClassPrivate), (void*)&objClass, sizeof(UObjectBase))){
            printf("error: access remote failed, objClass:0x%08x\n", obj.ClassPrivate);
            return;        
        }
        UObjectBase objOuter = { 0 };
        if(obj.OuterPrivate){
            if(0 != readNBytes(G_PID, (void*)(obj.OuterPrivate), (void*)&objOuter, sizeof(UObjectBase))){
                printf("error: access remote failed, objOuter:0x%08x\n", obj.OuterPrivate);
                return;        
            }            
        }
        
        char objName[256] = { 0 };
        char objClassName[256] = { 0 };
        char objOuterName[256] = { 0 };
        GetNameByIndex(objClass.NamePrivate.ComparisonIndex, objClassName);
        GetNameByIndex(obj.NamePrivate.ComparisonIndex, objName);
        GetNameByIndex(objOuter.NamePrivate.ComparisonIndex, objOuterName);

        printf("    VTableObject=0x%08x, ObjectFlags=0x%08x, InternalIndex=%d, ClassPrivate=0x%08x(%s), NamePrivate.ComparisonIndex=%d(%s), NamePrivate.Number=%d, OuterPrivate=0x%08x(%s)\n",\
                obj.VTableObject, 
                obj.ObjectFlags, 
                obj.InternalIndex, 
                obj.ClassPrivate,
                objClassName, 
                obj.NamePrivate.ComparisonIndex, 
                objName, 
                obj.NamePrivate.Number, 
                obj.OuterPrivate,
                obj.OuterPrivate? objOuterName : NULL);
        if(item.Object){
            printf("dump UObject:%p\n", item.Object);
            DumpPubgmhdClass::getInstance(G_PID, G_libUE4_base).sequence_traverse_dump((UObject*)item.Object);
        }

    }

}

typedef struct charmove{
    unsigned int remote_uobject;
    unsigned int remote_outer;
}charmove;

static std::map<unsigned int, charmove> STCharacterMovementComponent_map; // map<InternalIndex, remote obj and outer>
static unsigned int remote_BP_PlayerCameraManager_C;
void readLocationByMovementComp(){
    // &GUObjectArray.ObjObjects;
    if(!G_ADDR_GUObjectArray__ObjObjects){
        printf("error: G_ADDR_GUObjectArray__ObjObjects is NULL\n");
        return;
    }
    FFixedUObjectArray fixed_uobject_array = { 0 };
    // 用于发现被修改后的FFixedUObjectArray结构
    // unsigned int temp [10];
    // if(0 != readNBytes(G_PID, (void*)G_ADDR_GUObjectArray__ObjObjects, (void*)&temp, sizeof(temp))){
    //     printf("error: access temp\n");
    //     return;
    // }
    // for(int i=0; i<10; i++){
    //     printf("[%d]0x%08x\n", i, temp[i]);
    // }
    if(0 != readNBytes(G_PID, (void*)G_ADDR_GUObjectArray__ObjObjects, (void*)&fixed_uobject_array, sizeof(FFixedUObjectArray))){
        printf("error: access remote failed:0x%08x\n", G_ADDR_GUObjectArray__ObjObjects);
        return;
    }
    // MaxElements, NumElements看着不对，可能是偏移有问题。
    printf("Objects=0x%08x, MaxElements=%d, NumElements=%d\n", \
            fixed_uobject_array.Objects, fixed_uobject_array.MaxElements, fixed_uobject_array.NumElements);
    
    for(int i=0; i<fixed_uobject_array.NumElements; i++){
        FUObjectItem item = { 0 };
        if(0 != readNBytes(G_PID, (void*)(fixed_uobject_array.Objects+i), (void*)&item, sizeof(FUObjectItem))){
            printf("error: access remote failed:0x%08x\n", fixed_uobject_array.Objects);
            return;
        }
        // printf("[%d] Object=0x%08x, Flags=0x%08x, ClusterRootIndex=%d, SerialNumber=%d\n", \
        //         i, item.Object, item.Flags, item.ClusterRootIndex, item.SerialNumber);

        if(!item.Object)continue;
        
        UObjectBase obj = { 0 };
        if(0 != readNBytes(G_PID, (void*)(item.Object), (void*)&obj, sizeof(UObjectBase))){
            printf("error: access remote failed, Objects[%d].Object is 0x%08x\n", i, item.Object);
            return;        
        }
        UObjectBase objClass = { 0 };
        if(0 != readNBytes(G_PID, (void*)(obj.ClassPrivate), (void*)&objClass, sizeof(UObjectBase))){
            printf("error: access remote failed, objClass:0x%08x\n", obj.ClassPrivate);
            return;        
        }
        UObjectBase objOuter = { 0 };
        if(obj.OuterPrivate){
            if(0 != readNBytes(G_PID, (void*)(obj.OuterPrivate), (void*)&objOuter, sizeof(UObjectBase))){
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
        GetNameByIndex(obj.NamePrivate.ComparisonIndex, objName);
        GetNameByIndex(objClass.NamePrivate.ComparisonIndex, objClassName);
        GetNameByIndex(objOuter.NamePrivate.ComparisonIndex, objOuterName);
        // 收集类型为STCharacterMovementComponent的UObject对象
        if(
            strncmp(objClassName, "STCharacterMovementComponent", strlen("STCharacterMovementComponent")) == 0 
            && strncmp(objName, "CharMoveComp", strlen("CharMoveComp")) == 0
            && strncmp(objOuterName, "BP_TrainPlayerPawn_C", strlen("BP_TrainPlayerPawn_C")) == 0
        ) 
        {
            unsigned int InternalIndex = obj.InternalIndex;
            if(STCharacterMovementComponent_map.find(InternalIndex) == STCharacterMovementComponent_map.end()){
                printf("find a new CharMoveComp:STCharacterMovementComponent: 0x%08x\n", (unsigned int)item.Object);
                STCharacterMovementComponent_map[InternalIndex] = {(unsigned int)item.Object, (unsigned int)obj.OuterPrivate};
            }
        }

        if(strncmp(objName, "BP_PlayerCameraManager_C", strlen("BP_PlayerCameraManager_C")) == 0
            && strncmp(objClassName, "BP_PlayerCameraManager_C", strlen("BP_PlayerCameraManager_C")) == 0
            && strncmp(objOuterName, "PersistentLevel", strlen("PersistentLevel")) == 0
        ){
            remote_BP_PlayerCameraManager_C = (unsigned int)item.Object;
        }
    }
    printf("STCharacterMovementComponent_map size:%d\n", STCharacterMovementComponent_map.size());



    do{

        FMinimalViewInfo POV;
        char buf[0x590] = {0};
        unsigned int CameraCache_off = 0x350;
        unsigned int POV_off = CameraCache_off + 0x4;
        if(0 != readNBytes(G_PID, (void*)(remote_BP_PlayerCameraManager_C + CameraCache_off), (void*)buf, 0x590)){
            printf("error: access remote CameraCache failed:0x%08x, size:%d\n", (remote_BP_PlayerCameraManager_C + POV_off), 0x590);
            return;        
        }
        // float *pf = (float*)buf;
        // for(int i=0; i<0x590/4; i++){
        //     printf("[%d]%f ",i, *pf);
        //     pf++;
        //     if(i>0 && i%8 == 0){
        //         printf("\n");
        //     }
        // }
        // printf("\n");
        memcpy(&POV, buf+4, sizeof(FMinimalViewInfo));
        printf("POV Location=(%f,%f,%f) Rotation=(%f,%f,%f) FOV=%f, AspectRatio=%f\n",
            POV.Location.X, POV.Location.Y, POV.Location.Z, POV.Rotation.Yaw, POV.Rotation.Pitch, POV.Rotation.Roll,
            POV.FOV, POV.AspectRatio);

        FSceneViewProjectionData ProjectionData;
        MySimulation::LocalPlayer__GetProjectionData(2340.f, 1080.f, POV, ProjectionData);


        std::map<unsigned int, charmove>::iterator it;
        for(it=STCharacterMovementComponent_map.begin(); it != STCharacterMovementComponent_map.end(); ++it){
            unsigned int InternalIndex = it->first;
            unsigned int p_CharMoveComp = (it->second).remote_uobject;
            unsigned int remote_Outer = it->second.remote_outer;

            FVector LastUpdateLocation;
            if(0 != readNBytes(G_PID, (void*)(p_CharMoveComp + 0x27c), (void*)&LastUpdateLocation, sizeof(FVector))){
                printf("error: access remote LastUpdateLocation failed:0x%08x, size:%d\n",p_CharMoveComp + 0x27c, sizeof(FVector) );
                return;        
            }

            UObject temp_Outer;
            if(0 != readNBytes(G_PID, (void*)remote_Outer, (void*)&temp_Outer, sizeof(UObject))){
                printf("error: access remote Outer failed:0x%08x, size:%d\n",remote_Outer, sizeof(UObject) );
                return;        
            }
            TArray<wchar_t> PlayerName;
            if(0 != readNBytes(G_PID, (void*)(remote_Outer+0x5f8), (void*)&PlayerName, sizeof(PlayerName))){
                printf("error: access remote PlayerName failed:0x%08x, size:%d\n",remote_Outer+0x5f8, sizeof(PlayerName) );
                return;        
            }

            FVector2D out_ScreenPos;
            FSceneView::ProjectWorldToScreen(LastUpdateLocation, ProjectionData.GetViewRect(), ProjectionData.ComputeViewProjectionMatrix(), out_ScreenPos);

            if(PlayerName.Data){
                // Note: 和平精英游戏里的PlayerName.Data存储的每个字符占用2个字节
                // 在Android里，sizeof(wchar_t)=4，因此先读取2 * PlayerName.Count个字节，
                // 要在printf里打印，需要将逐个将字符的2个字节赋值给wchar_t字符数组，再调用printf打印
                uint16_t temp_playername[256] = { 0 };
                //printf("sizeof(wchar_t)=%d, read len:%d", sizeof(wchar_t), 2 * PlayerName.Count);
                if(0 != readNBytes(G_PID, (void*)PlayerName.Data, (void*)&temp_playername, 2*PlayerName.Count)){
                    printf("error: access remote temp_playername failed:0x%08x, size:%d\n", PlayerName.Data, 2*PlayerName.Count);
                    return;        
                }
                wchar_t temp_wchar[256] = {0};
                for(int i=0; i<PlayerName.Count; i++){
                    //printf("[%d] %04x ", i, temp_playername[i]);
                    temp_wchar[i] = temp_playername[i];
                }
                //printf("\n");
                setlocale(LC_ALL, "");
                // if(printf("%ls\n", temp_wchar) < 0){
                //         perror("printf");
                // }
                printf("InternalIndex=%d, p_CharMoveComp=0x%08x, LastUpdateLocation=(%f,%f,%f)  screenPos:(%f,%f) %ls\n", 
                    InternalIndex, p_CharMoveComp, LastUpdateLocation.X, LastUpdateLocation.Y, LastUpdateLocation.Z,
                    out_ScreenPos.X, out_ScreenPos.Y, temp_wchar);     
            }
            else{
                printf("InternalIndex=%d, p_CharMoveComp=0x%08x, LastUpdateLocation=(%f,%f,%f)\n", 
                    InternalIndex, p_CharMoveComp, LastUpdateLocation.X, LastUpdateLocation.Y, LastUpdateLocation.Z);  
            }

            
 
        }

// /** 
//  * Cached camera POV info, stored as optimization so we only
//  * need to do a full camera update once per tick.
//  */
// USTRUCT()
// struct FCameraCacheEntry
// {
// 	GENERATED_USTRUCT_BODY()
// public:

// 	/** World time this entry was created. */
// 	UPROPERTY()
// 	float TimeStamp;

// 	/** Camera POV to cache. */
// 	UPROPERTY()
// 	FMinimalViewInfo POV;

// 	FCameraCacheEntry()
// 		: TimeStamp(0.f)
// 	{}
// };



        printf("-----\n");

        sleep(5);
    }while(true);    
}

int print_player_name(UObject *pUObj){
    // assume pUObj is a player
    
    return 0;
}

static struct TNameEntryArray *Names = NULL;

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
    G_libUE4_base = libUE4_base;
    
    G_ADDR_NAMES = libUE4_base + G_OFF_NAMES;
    printf("libUE4.so base=0x%08x, G_ADDR_NAMES = 0x%08x + 0x%08x = 0x%08x\n", libUE4_base, libUE4_base, G_OFF_NAMES, G_ADDR_NAMES);
    Names = (struct TNameEntryArray*)G_ADDR_NAMES;

    G_ADDR_GUObjectArray__ObjObjects = libUE4_base + G_OFf_GUObjectArray__ObjObjects;
    printf("libUE4.so base=0x%08x, G_ADDR_GUObjectArray__ObjObjects = 0x%08x + 0x%08x = 0x%08x\n", \
            libUE4_base, libUE4_base, G_OFf_GUObjectArray__ObjObjects, G_ADDR_GUObjectArray__ObjObjects);
    
    char s[1024] = { 0 };
    GetNameByIndex(1199, s);
    printf("s:%s\n", s);
    //readObjects();
    readLocationByMovementComp();

    //char ws[] = {0x60, 0x4f, 0x28, 0x57, 0x93, 0x62, 0x50, 0x5b, 0x6c, 0x70, 0xa8, 0x61, 0x79, 0x62, 0x00, 0x00};
    wchar_t ws[] = {0x4f60, 0x5728, 0x6293, 0x5b50, 0x706c, 0x61a8, 0x6279, 0};
    wchar_t ws2[] = {0x5e05, 0x6c14, 0x8def, 0x838e, 0x0034, 0x0037, 0x0033, 0};
    setlocale(LC_ALL, "");
    printf("ws:%ls\n", ws2);
    return 0;
}