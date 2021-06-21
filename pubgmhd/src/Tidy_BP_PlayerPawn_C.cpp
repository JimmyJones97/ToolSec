#include <sstream>
#include <string>
#include "RemoteUtils.h"
#include "Tidy_BP_PlayerPawn_C.h"

extern pid_t G_PID;

bool Tidy_BP_PlayerPawn_C::SyncFromRemote(){
    bool success = true;
    if(0 != readNBytes(G_PID, (void*)(RemoteBaseAddr+OFF_IsPossessed), (void*)&IsPossessed, sizeof(IsPossessed))){
        DEBUG_PRINT("read IsPossessed failed");
        success = false;
    }

    if(0 != readNBytes(G_PID, (void*)(RemoteBaseAddr+OFF_TeamID), (void*)&TeamID, sizeof(TeamID))){
        DEBUG_PRINT("read TeamID failed");
        success = false;
    }

    if(0 != readNBytes(G_PID, (void*)(RemoteBaseAddr+OFF_bIsAI), (void*)&bIsAI, sizeof(bIsAI))){
        DEBUG_PRINT("read bIsAI failed");
        success = false;
    }

    if(0 != readNBytes(G_PID, (void*)(RemoteBaseAddr+OFF_Health), (void*)&Health, sizeof(Health))){
        DEBUG_PRINT("read Health failed");
        success = false;
    }

    if(0 != readNBytes(G_PID, (void*)(RemoteBaseAddr+OFF_HealthMax), (void*)&HealthMax, sizeof(HealthMax))){
        DEBUG_PRINT("read HealthMax failed");
        success = false;
    }

    if(0 != readNBytes(G_PID, (void*)(RemoteBaseAddr+OFF_bDead), (void*)&bDead, sizeof(bDead))){
        DEBUG_PRINT("read bDead failed");
        success = false;
    }
    bDead &= 1;

    if(0 != readNBytes(G_PID, (void*)(RemoteBaseAddr+OFF_TeamNum), (void*)&TeamNum, sizeof(TeamNum))){
        DEBUG_PRINT("read TeamNum failed");
        success = false;
    }

    if(0 != readNBytes(G_PID, (void*)(RemoteBaseAddr+OFF_bDying), (void*)&bDying, sizeof(bDying))){
        DEBUG_PRINT("read bDying failed");
        success = false;
    }
    bDying &= 1;

    if(0 != readNBytes(G_PID, (void*)(RemoteBaseAddr+OFF_Controller), (void*)&Controller, sizeof(Controller))){
        DEBUG_PRINT("read Controller failed");
        success = false;
    }
    if(Controller){
        if(0 != readNBytes(G_PID, (void*)(Controller+OFF_bIsLocalPlayerController), (void*)&bIsLocalPlayerController, sizeof(bIsLocalPlayerController))){
            DEBUG_PRINT("read bIsLocalPlayerController failed");
            success = false;
        }
    }
    else{
        bIsLocalPlayerController = 0;
    }


    return success;
}

Tidy_BP_PlayerPawn_C::Tidy_BP_PlayerPawn_C(unsigned long remote_p_BP_PlayerPawn_C){
    RemoteBaseAddr = remote_p_BP_PlayerPawn_C;
    SyncOK = SyncFromRemote(); 
}

void Tidy_BP_PlayerPawn_C::Update(){
    SyncOK = SyncFromRemote();
}

std::string Tidy_BP_PlayerPawn_C::ToString(){
    std::string s;
    std::ostringstream oss;
    oss << "SyncOK:" << std::to_string(SyncOK) \
        << " IsPossessed:" << std::to_string(IsPossessed)\
        << " TeamID:" << std::to_string(TeamID)\
        << " bIsAI:" << std::to_string(bIsAI)\
        << " Health:" << std::to_string(Health) << "/" << std::to_string(HealthMax)\
        << " bDead:" << std::to_string(bDead)\
        << " TeamNum:" << std::to_string(TeamNum)\
        << " bDying:" << std::to_string(bDying)\
        << " Controller:0x" << std::hex << Controller\
        << " bIsLocalPlayerController:" << std::to_string(bIsLocalPlayerController)\
        << std::endl;
    s = oss.str();
    return s;
}