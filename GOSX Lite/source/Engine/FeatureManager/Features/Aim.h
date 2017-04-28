//
//  Aim.hpp
//  GOSX Lite
//
//  Created by Andre Kalisch on 30.03.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#ifndef Aim_hpp
#define Aim_hpp

#include "SDK/SDK.h"
#include "SDK/CCSPlayer.h"
#include "Engine/WeaponManager/weapons.h"

class CAim {
public:
    CAim();
    C_CSPlayer* FindTarget(C_CSPlayer* LocalPlayer);
    void StartAim(C_CSPlayer* LocalPlayer, C_CSPlayer* AimTarget, CUserCmd* pCmd);
    void CreateMove(CUserCmd* pCmd);
    static std::map<int, const char*> GetBoneList();
    static std::map<int, const char*> GetBoneValues();
    bool HasTarget();
protected:
    void Smooth(QAngle& angle);
    void AutoCrouch(C_CSPlayer* LocalPlayer, C_CSPlayer* EntityPlayer, CUserCmd* cmd, bool inAttack = false);
    void AutoStop(C_CSPlayer* LocalPlayer, C_CSPlayer* EntityPlayer, CUserCmd* cmd);
    void AutoShoot(C_CSPlayer* player, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd);
    void RecoilControl(QAngle& angle, C_CSPlayer* LocalPlayer, C_CSPlayer* player, CUserCmd* cmd, bool inAttack = false);
    int MakeHitscan(C_CSPlayer* pLocal, C_CSPlayer* pEntity);
    void Reset();
private:
    C_CSPlayer* LockedTargetEntity = nullptr;
    int LockedEntityBone = -5;
    bool HasValidTargetAndIsAiming = false;
    std::vector<int> hitboxes;
};

#endif /* Aim_hpp */
