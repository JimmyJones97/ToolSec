#include <stdio.h>

#include "UE4Names.h"
#include "EngineClasses.h"
#include "RemoteUtils.h"

extern unsigned long G_ADDR_NAMES;
extern pid_t G_PID;

char* GetNameByIndex(int Index, char *ret)
{
    
    if(!G_ADDR_NAMES){
        printf("error: G_ADDR_NAMES is NULL\n");
        return NULL;
    }

    struct TNameEntryArray *Names = NULL;
    if(NULL != readNBytes(G_PID, (void*)G_ADDR_NAMES, &Names, 4)){
        printf("error: access remote failed:0x%x\n", G_ADDR_NAMES);
        return NULL;
    }
    //printf("[dword_6E6B164] = [0x%08x] = 0x%08x = Names\n", G_ADDR_NAMES, (unsigned long)Names);    

    struct TNameEntryArray name_entry_array = { 0 };
    if(0 != readNBytes(G_PID, (void*)Names, (void*)&name_entry_array, sizeof(struct TNameEntryArray))){
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
    if(0 != readNBytes(G_PID, (void*)ItemPtr, (void*)&addr_name_entry, 4)){
        printf("error: access remote failed:0x%08x\n", (unsigned long)ItemPtr);
        return NULL;        
    }
    //printf("addr_name_entry=0x%08x\n", addr_name_entry);
    if(!addr_name_entry)return NULL;

    FNameEntry name_entry = { 0 };
    if(0 != readNBytes(G_PID, (void*)addr_name_entry, (void*)&name_entry, sizeof(FNameEntry))){
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
    //         if(0 != readNBytes(G_PID, (void*)ItemPtr, (void*)&addr_name_entry, 4)){
    //             printf("error: access remote failed:0x%08x\n", (unsigned long)ItemPtr);
    //             return NULL;        
    //         }
    //         printf("addr_name_entry=0x%08x\n", addr_name_entry);
    //         if(!addr_name_entry)break;

    //         FNameEntry name_entry = { 0 };
    //         if(0 != readNBytes(G_PID, (void*)addr_name_entry, (void*)&name_entry, sizeof(FNameEntry))){
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