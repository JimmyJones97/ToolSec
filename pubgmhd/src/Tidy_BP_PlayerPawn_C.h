#ifndef MINIMAL_BP_PLAYERPAWN_C_H
#define MINIMAL_BP_PLAYERPAWN_C_H
#include <stdint.h>
#include "OFF_BP_PlayerPawn_C.h"

class Tidy_BP_PlayerPawn_C{
public:
    bool                SyncOK;
    unsigned long       RemoteBaseAddr;

    uint8_t             IsPossessed;
    int32_t             TeamID;
    uint8_t             bIsAI;
    float               Health;
    float               HealthMax;
    uint32_t            bDead;
    uint8_t             TeamNum;
    uint8_t             bDying;

    uint32_t            STPlayerController;
    uint32_t            Controller;
    uint8_t             bIsLocalPlayerController;

public:
    Tidy_BP_PlayerPawn_C(unsigned long remote_p_BP_PlayerPawn_C);
    void Update();
    std::string ToString();
private:
    bool SyncFromRemote();
};

#endif