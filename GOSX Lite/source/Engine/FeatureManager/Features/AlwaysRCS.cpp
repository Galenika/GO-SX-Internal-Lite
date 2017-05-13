//
//  AlwaysRCS.cpp
//  GOSX Lite
//
//  Created by Andre Kalisch on 04.04.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "AlwaysRCS.h"

CAlwaysRCS::CAlwaysRCS(std::shared_ptr<CAim> aimbot) {
    if (!aimInstance) {
        aimInstance = aimbot;
    }
}

void CAlwaysRCS::apply(CUserCmd *pCmd) {
    if(pCmd->buttons & IN_ATTACK) {
        if(INIGET_BOOL("AimHelper", "aim_rcs") && aimInstance->HasTarget()) {
            lastRCSPunch = {0.f, 0.f, 0.f};
            return;
        }

        QAngle ViewAngle = pCmd->viewangles;

        C_CSPlayer* LocalPlayer = C_CSPlayer::GetLocalPlayer();
        if (!LocalPlayer || !LocalPlayer->IsValidLivePlayer()) {
            lastRCSPunch = {0.f, 0.f, 0.f};
            return;
        }

        C_BaseCombatWeapon* activeWeapon = LocalPlayer->GetActiveWeapon();
        if (!activeWeapon || !CWeaponManager::isRCSWeapon(activeWeapon->GetWeaponEntityID())) {
            lastRCSPunch = {0.f, 0.f, 0.f};
            return;
        }

        QAngle CurrentAimPunch = LocalPlayer->AimPunch();
        float RCSLevel = INIGET_FLOAT("AimHelper", "aim_rcs_level");
        if (CurrentAimPunch.x != 0.0f || CurrentAimPunch.y != 0.0f) {
            QAngle NewPunch = {CurrentAimPunch.x - lastRCSPunch.x, CurrentAimPunch.y - lastRCSPunch.y, 0.f};

            ViewAngle.x -= NewPunch.x * RCSLevel;
            ViewAngle.y -= NewPunch.y * RCSLevel;

            CMath::NormalizeAngles(ViewAngle);
            CMath::ClampAngle(ViewAngle);

            Interfaces::Engine()->SetViewAngles(ViewAngle);
            lastRCSPunch = CurrentAimPunch;
        } else {
            lastRCSPunch = {0.f, 0.f, 0.f};
        }
    } else {
        lastRCSPunch = {0.f, 0.f, 0.f};
    }
}
