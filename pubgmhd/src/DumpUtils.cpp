#include "DumpUtils.h"
#include "UE4Names.h"
#include "logprint.hpp"
#include "RemoteUtils.h"
#include "EngineClasses.h"

#define READ_N_BYTES(PID, RBASE, DATA, DATATYPE, DATAPOINTER) do{\
    if(0 != readNBytes(PID, (void*)RBASE, (void*)&DATA, sizeof(DATATYPE))){\
        DEBUG_PRINT("read remote failed, remote_pid:%d, rbase:%p", PID, RBASE);\
        return -1;\
    }\
    DATAPOINTER = (DATATYPE*)(&DATA);\                
}while(0);

int DumpPubgmhdClass::sequence_traverse_dump(UObject* q, int dig_lvl){
        if(!q){
            DEBUG_PRINT("UObject* q is NULL");
            return -1;
        }

        std::vector<UObject*> remote_obj_addr_list;
        // 当前UObject
        UObject localCurUObject;
        UObject *pLocalCurUObject;
        // 当前UObject的ClassPrivate
        UClass localCurUObjectUClass;
        UClass *pLocalCurUObjectUClass;

        // 当前SuperStruct的UField
        UField localCurSuperStructUField;
        // 当前SuperStruct的UField的ClassPrivate
        UClass localCurSuperStructUFieldUClass;

        UField curObjChild;
        UObjectProperty curObjObjectProperty;
        UObject curObjObjectPropertyObj;
        UProperty curObjProperty;
        UFunction curObjFunction;
        UMulticastDelegateProperty curObjUMulticastDelegateProperty;
        
        
        
        std::string key;
        std::map<std::string, int>::iterator it;
        char temp_obj_name[256] = {0};
        char temp_obj_class_name[256] = {0};
        char temp_outer_name[256] = {0};
        char temp_outer_class_name[256] = {0};
        char temp_obj_inherit_class_name[256] = {0};
        char temp_field_class_name[256] = {0};
        char temp_field_name[256] = {0};
        
        remote_obj_addr_list.push_back(q);
        while(!remote_obj_addr_list.empty()){
            UObject* pRemoteCurUObject = remote_obj_addr_list.back();
            remote_obj_addr_list.pop_back();
            if(!pRemoteCurUObject){
                DEBUG_PRINT("Skip pRemoteCurUObject: %p", pRemoteCurUObject);
                continue;
            }
            // 远程读取UObject
            if(0 != readData<UObject>(remote_pid, (void*)pRemoteCurUObject, localCurUObject)){
                DEBUG_PRINT("read localCurUObject failed");
                return -1;
            }
            pLocalCurUObject = (UObject*)&localCurUObject;

            // 远程读取UClass
            UClass *pRemoteUClass = pLocalCurUObject->ClassPrivate;
            // ClassPrivate应该必然不为空
            if(0 != readData<UClass>(remote_pid, (void*)pRemoteUClass, localCurUObjectUClass)){
                DEBUG_PRINT("read localCurUObjectUClass failed");
                return -1;
            }            
            pLocalCurUObjectUClass = (UClass*)&localCurUObjectUClass;

            key = GetNameByIndex(pLocalCurUObjectUClass->NamePrivate.ComparisonIndex, temp_obj_class_name);
            it = mDumpedClass.find(key);
            if(it == mDumpedClass.end()){ // 当前未保存这个类
                GetNameByIndex(pLocalCurUObject->NamePrivate.ComparisonIndex, temp_obj_name);
                DEBUG_PRINT(">> Start to dump: %s(class: %s)", temp_obj_name, key.c_str());
                std::string classInfo;
                unsigned long *remoteVTablePointer = (unsigned long*)pRemoteCurUObject;
                unsigned long VTable;
                unsigned long VTable_offset;

                UField *local_p_UField;
                char *fieldcls;
                char *fieldname;
                UProperty *pUProperty;
                UFunction *pUFunction;
                int lv = 0;
                char line[1024];
                // 读虚表地址
                if(0 != readNBytes(remote_pid, (void*)remoteVTablePointer, (void*)&VTable, sizeof(unsigned long))){
                    DEBUG_PRINT("read VTable failed");
                    return -1;
                }                    
                VTable_offset = VTable - this->libUE4_base;
                GetNameByIndex(pLocalCurUObject->NamePrivate.ComparisonIndex, temp_obj_name);
                GetNameByIndex(pLocalCurUObjectUClass->NamePrivate.ComparisonIndex, temp_obj_class_name);
                DEBUG_PRINT("VTable of %s(class: %s): %08x, %08x", temp_obj_name, temp_obj_class_name, VTable, VTable_offset);
                sprintf(line, "VTable of %s: %#lx, %#lx\n\n", temp_obj_class_name, VTable, VTable_offset);
                classInfo.append(line);
                
                // 查看从父类继承的成员
                UStruct *remote_p_SuperStruct = pLocalCurUObjectUClass->SuperStruct;
                DEBUG_PRINT("remote_p_SuperStruct:%p", pLocalCurUObjectUClass->SuperStruct);

                while(remote_p_SuperStruct){
                    UStruct temp_SuperStruct;
                    if(0 != readData<UStruct>(remote_pid, (void*)remote_p_SuperStruct, temp_SuperStruct)){
                        DEBUG_PRINT("read temp_SuperStruct failed");
                        return -1;
                    }
                    UStruct *local_p_SuperStruct = (UStruct*)&temp_SuperStruct;  
                                
                    GetNameByIndex(local_p_SuperStruct->NamePrivate.ComparisonIndex, temp_obj_inherit_class_name);
                    DEBUG_PRINT(">> start of Class: %s", temp_obj_inherit_class_name);
                    sprintf(line, ">> start of Class: %s\n", temp_obj_inherit_class_name);
                    classInfo.append(line);
                    lv++;

                    // 遍历当前父类的成员属性
                    UField *remoteUField = local_p_SuperStruct->Children;
                    DEBUG_PRINT("remoteUField of local_p_SuperStruct->Children: %p", remoteUField);
                    while(remoteUField)
                    {
                        UField temp_UField;
                        if(0 != readData<UField>(remote_pid, (void*)remoteUField, temp_UField)){
                            DEBUG_PRINT("read temp_UField failed");
                            return -1;
                        }
                        UField *local_p_UField = (UField*)&temp_UField;  

                        
                        UClass temp_UClass;
                        if(0 != readData<UField>(remote_pid, (void*)(local_p_UField->ClassPrivate), temp_UClass)){
                            DEBUG_PRINT("read temp_UClass failed");
                            return -1;
                        }
                        UClass *local_p_UClass = (UClass*)&temp_UClass; 
                        
                        fieldcls = GetNameByIndex(local_p_UClass->NamePrivate.ComparisonIndex, temp_field_class_name);
                        fieldname = GetNameByIndex(local_p_UField->NamePrivate.ComparisonIndex, temp_field_name);
                        //DEBUG_PRINT("fieldcls: %s, fieldname: %s", fieldcls, fieldname);

                        // 解析ObjectProperty成员
                        if(strcmp(fieldcls, "ObjectProperty") == 0){
                            UObjectProperty temp_UObjectProperty;
                            if(0 != readData<UObjectProperty>(remote_pid, (void*)remoteUField, temp_UObjectProperty)){
                                DEBUG_PRINT("read temp_UObjectProperty failed");
                                return -1;
                            }                                   
                            UObjectProperty *local_p_UObjectProperty = (UObjectProperty*)&temp_UObjectProperty; 

                            sprintf(line, "[Inherit:%d] %30s %-50s  // Offset_Internal: %#04x, ArrayDim:%d, ElementSize: %#04x \n",
                                lv, fieldcls, fieldname, 
                                local_p_UObjectProperty->Offset_Internal, 
                                local_p_UObjectProperty->ArrayDim, 
                                local_p_UObjectProperty->ElementSize
                                );
                            classInfo.append(line);

                            // 将当前UObject的Field入队（也是一个UObject)
                            unsigned long remote_Field_addr = (unsigned long)pLocalCurUObject + local_p_UObjectProperty->Offset_Internal;
                            if(dig_lvl>0){
                                remote_obj_addr_list.push_back((UObject*)remote_Field_addr);
                            }
                        }
                        // 解析UProperty成员
                        else if(strstr(fieldcls, "Property") && strcmp(fieldcls, "MulticastDelegateProperty")!=0){ // the field inherit from UProperty
                            UProperty temp_UProperty;
                            if(0 != readData<UProperty>(remote_pid, (void*)remoteUField, temp_UProperty)){
                                DEBUG_PRINT("read temp_UProperty failed");
                                return -1;
                            }                                  
                            UProperty *local_p_UProperty = (UProperty*)&temp_UProperty; 
                            sprintf(line, "[Inherit:%d] %30s %-50s  // Offset_Internal: %#04x, ArrayDim:%d, ElementSize: %#04x\n",
                                lv, fieldcls, fieldname, 
                                local_p_UProperty->Offset_Internal, 
                                local_p_UProperty->ArrayDim, 
                                local_p_UProperty->ElementSize);
                            classInfo.append(line);
                        }
                        // 解析Function成员
                        else if(strstr(fieldcls, "Function")){
                            UFunction temp_UFunction;
                            if(0 != readData<UFunction>(remote_pid, (void*)remoteUField, temp_UFunction)){
                                DEBUG_PRINT("read temp_UFunction of UFunction inherit failed");
                                return -1;
                            }
                            UFunction *local_p_UFunction = (UFunction*)&temp_UFunction;
                            sprintf(line, "[Inherit:%d] %30s %-50s  // offset:%#08x\n", 
                                lv, fieldcls, fieldname, 
                                (unsigned long)(local_p_UFunction->Func) - (unsigned long)libUE4_base);
                            classInfo.append(line);
                        }
                        // // 解析UMulticastDelegateProperty成员
                        // else if(strcmp(fieldcls, "MulticastDelegateProperty") == 0){
                        //     UMulticastDelegateProperty temp_UMulticastDelegateProperty;
                        //     if(0 != readData<UMulticastDelegateProperty>(remote_pid, (void*)remoteUField, temp_UMulticastDelegateProperty)){
                        //         DEBUG_PRINT("read temp_UMulticastDelegateProperty failed");
                        //         return -1;
                        //     }   
                        //     UMulticastDelegateProperty *local_p_UMulticastDelegateProperty = (UMulticastDelegateProperty*)&temp_UMulticastDelegateProperty;
                        //     UFunction *remote_p_UFunction = local_p_UMulticastDelegateProperty->SignatureFunction;
                        //     if(remote_p_UFunction){
                        //         // FIXME: remote_p_UFunction地址错误
                        //         UFunction temp_UFunction;
                        //         if(0 != readData<UFunction>(remote_pid, (void*)remote_p_UFunction, temp_UFunction)){
                        //             DEBUG_PRINT("read temp_UFunction of SignatureFunction inherit failed");
                        //             return -1;
                        //         }
                        //         UFunction *local_p_UFunction = (UFunction*)&temp_UFunction; 
                        //         sprintf(line, "[Inherit:%d] %30s %-50s  // offset:%#08x\n", 
                        //             lv, fieldcls, fieldname, 
                        //             (unsigned long)(local_p_UFunction->Func) - (unsigned long)libUE4_base);
                        //         classInfo.append(line);
                        //     }
                        // }
                        else{
                            sprintf(line, "[Inherit:%d] %30s %-50s\n", lv, fieldcls, fieldname);
                            classInfo.append(line);
                        }
                        remoteUField = local_p_UField->Next;
                    }
                
                    DEBUG_PRINT("remote_p_SuperStruct = local_p_SuperStruct->SuperStruct = %p", local_p_SuperStruct->SuperStruct);
                    remote_p_SuperStruct = local_p_SuperStruct->SuperStruct;
                }
                

                
                // 查看类本身的成员
                sprintf(line, ">> start of Class: %s\n", temp_obj_class_name);
                classInfo.append(line);
                UField *remoteUField = pLocalCurUObjectUClass->Children;
                DEBUG_PRINT("remoteUField of pLocalCurUObjectUClass: %p", remoteUField);
                while(remoteUField)
                {
                    UField temp_UField;
                    if(0 != readData<UField>(remote_pid, (void*)remoteUField, temp_UField)){
                        DEBUG_PRINT("read temp_UField failed");
                        return -1;
                    }
                    UField *local_p_UField = (UField*)&temp_UField;  

                    UClass temp_UClass;
                    if(0 != readData<UField>(remote_pid, (void*)(local_p_UField->ClassPrivate), temp_UClass)){
                        DEBUG_PRINT("read temp_UClass failed");
                        return -1;
                    }
                    UClass *local_p_UClass = (UClass*)&temp_UClass; 
                    
                    fieldcls = GetNameByIndex(local_p_UClass->NamePrivate.ComparisonIndex, temp_field_class_name);
                    fieldname = GetNameByIndex(local_p_UField->NamePrivate.ComparisonIndex, temp_field_name);
                    //DEBUG_PRINT("fieldcls: %s, fieldname: %s", fieldcls, fieldname);

                    // 解析ObjectProperty成员
                    if(strcmp(fieldcls, "ObjectProperty") == 0){
                        UObjectProperty temp_UObjectProperty;
                        if(0 != readData<UObjectProperty>(remote_pid, (void*)remoteUField, temp_UObjectProperty)){
                            DEBUG_PRINT("read temp_UObjectProperty failed");
                            return -1;
                        }                                   
                        UObjectProperty *local_p_UObjectProperty = (UObjectProperty*)&temp_UObjectProperty; 

                        sprintf(line, "%30s %-50s  // Offset_Internal: %#04x, ArrayDim:%d, ElementSize: %#04x \n",
                            fieldcls, fieldname, 
                            local_p_UObjectProperty->Offset_Internal, 
                            local_p_UObjectProperty->ArrayDim, 
                            local_p_UObjectProperty->ElementSize
                            );
                        classInfo.append(line);

                        // 将当前UObject的Field入队（也是一个UObject)
                        unsigned long remote_Field_addr = (unsigned long)pLocalCurUObject + local_p_UObjectProperty->Offset_Internal;
                        if(dig_lvl>0){
                            remote_obj_addr_list.push_back((UObject*)remote_Field_addr);
                        }
                    }
                    // 解析UProperty成员
                    else if(strstr(fieldcls, "Property") && strcmp(fieldcls, "MulticastDelegateProperty")!=0){ // the field inherit from UProperty
                        UProperty temp_UProperty;
                        if(0 != readData<UProperty>(remote_pid, (void*)remoteUField, temp_UProperty)){
                            DEBUG_PRINT("read temp_UProperty failed");
                            return -1;
                        }                                  
                        UProperty *local_p_UProperty = (UProperty*)&temp_UProperty; 
                        sprintf(line, "%30s %-50s  // Offset_Internal: %#04x, ArrayDim:%d, ElementSize: %#04x\n",
                            fieldcls, fieldname, 
                            local_p_UProperty->Offset_Internal, 
                            local_p_UProperty->ArrayDim, 
                            local_p_UProperty->ElementSize);
                        classInfo.append(line);
                    }
                    // 解析Function成员
                    else if(strstr(fieldcls, "Function")){
                        UFunction temp_UFunction;
                        if(0 != readData<UFunction>(remote_pid, (void*)remoteUField, temp_UFunction)){
                            DEBUG_PRINT("read temp_UFunction of Function failed");
                            return -1;
                        }
                        UFunction *local_p_UFunction = (UFunction*)&temp_UFunction;
                        sprintf(line, "%30s %-50s  // offset:%#08x\n", 
                            fieldcls, fieldname, 
                            (unsigned long)(local_p_UFunction->Func) - (unsigned long)libUE4_base);
                        classInfo.append(line);
                    }
                    // // 解析UMulticastDelegateProperty成员
                    // else if(strcmp(fieldcls, "MulticastDelegateProperty") == 0){
                    //     UMulticastDelegateProperty temp_UMulticastDelegateProperty;
                    //     if(0 != readData<UMulticastDelegateProperty>(remote_pid, (void*)remoteUField, temp_UMulticastDelegateProperty)){
                    //         DEBUG_PRINT("read temp_UMulticastDelegateProperty failed");
                    //         return -1;
                    //     }   
                    //     UMulticastDelegateProperty *local_p_UMulticastDelegateProperty = (UMulticastDelegateProperty*)&temp_UMulticastDelegateProperty;
                    //     UFunction *remote_p_UFunction = local_p_UMulticastDelegateProperty->SignatureFunction;
                    //     if(remote_p_UFunction){
                    //         // FIXME: remote_p_UFunction地址错误
                    //         UFunction temp_UFunction;
                    //         if(0 != readData<UFunction>(remote_pid, (void*)remote_p_UFunction, temp_UFunction)){
                    //             DEBUG_PRINT("read temp_UFunction of SignatureFunction failed");
                    //             return -1;
                    //         }
                    //         UFunction *local_p_UFunction = (UFunction*)&temp_UFunction; 
                    //         sprintf(line, "%30s %-50s  // offset:%#08x\n", 
                    //             fieldcls, fieldname, 
                    //             (unsigned long)(local_p_UFunction->Func) - (unsigned long)libUE4_base);
                    //         classInfo.append(line);
                    //     }
                    // }
                    else{
                        sprintf(line, "%30s %-50s\n", fieldcls, fieldname);
                        classInfo.append(line);
                    }
                    remoteUField = local_p_UField->Next;
                }
                
                mDumpedClass[key] = 1;
                
                char fpath[256];
                GetNameByIndex(pLocalCurUObjectUClass->NamePrivate.ComparisonIndex, temp_obj_class_name);
                sprintf(fpath, "/sdcard/pubgmhdClass/%s.txt", temp_obj_class_name);
                FILE* f = fopen(fpath, "w+");
                if(f){
                    fwrite(classInfo.c_str(), strlen(classInfo.c_str()), 1, f);
                    fclose(f);
                    DEBUG_PRINT("dump finish: %s", fpath);
                }
                DEBUG_PRINT("------------------------");
            }
            dig_lvl -= 1;
        }
        return 0;
    }

int DumpPubgmhdClass::dump_specified_uobject(UObject *p_UObj){
    if(!p_UObj){
        DEBUG_PRINT("dump_specified_uobject error: p_UObj is NULL");
        return -1;
    }

    return 0;
}