//
//  AlwaysRCS.cpp
//  GOSX Lite
//
//  Created by Andre Kalisch on 04.04.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "AlwaysRCS.h"

CAlwaysRCS::CAlwaysRCS(CAim* aimbot) {
    if(!aimInstance) {
        aimInstance = aimbot;
    }
}

void CAlwaysRCS::apply(CUserCmd *pCmd) {
    if(pCmd->buttons & IN_ATTACK) {
        if(INIGET_BOOL("AimHelper", "aim_rcs") && aimInstance->HasTarget()) {
            return;
        }

        QAngle OldViewAngle = pCmd->viewangles;
        QAngle RCSViewAngle;

        C_CSPlayer* LocalPlayer = C_CSPlayer::GetLocalPlayer();
        if (!LocalPlayer || !LocalPlayer->IsValidLivePlayer()) {
            return;
        }

        C_BaseCombatWeapon* activeWeapon = LocalPlayer->GetActiveWeapon();
        if (!activeWeapon || !CWeaponManager::isRCSWeapon(activeWeapon->GetWeaponEntityID())) {
            return;
        }

        QAngle CurrentAimPunch = LocalPlayer->AimPunch();
        float RCSLevel = INIGET_FLOAT("AimHelper", "aim_rcs_level");
        if (CurrentAimPunch.x != 0.0f || CurrentAimPunch.y != 0.0f) {
            QAngle AimPunch = {CurrentAimPunch.x - lastRCSPunch.x, CurrentAimPunch.y - lastRCSPunch.y, 0.0f};

            RCSViewAngle.x = OldViewAngle.x - (AimPunch.x * RCSLevel);
            RCSViewAngle.y = OldViewAngle.y - (AimPunch.y * RCSLevel);
            RCSViewAngle.z = 0.0f;

            if (RCSViewAngle != OldViewAngle) {
                CMath::SmoothAngle(OldViewAngle, RCSViewAngle, (INIGET_FLOAT("AimHelper", "aim_smoothing") * 100));
                CMath::NormalizeAngles(RCSViewAngle);
                CMath::ClampAngle(RCSViewAngle);

                if (INIGET_BOOL("AimHelper", "aim_silent")) {
                    pCmd->viewangles = RCSViewAngle;
                } else {
                    Interfaces::Engine()->SetViewAngles(RCSViewAngle);
                }
            }
        }
        lastRCSPunch = CurrentAimPunch;
    } else {
        if(lastRCSPunch.x != 0.0f && lastRCSPunch.y != 0.0f) {
            lastRCSPunch = Vector(0.0f, 0.0f, 0.0f);
        }
    }
}
