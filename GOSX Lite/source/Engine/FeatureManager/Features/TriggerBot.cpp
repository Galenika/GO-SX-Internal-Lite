//
//  TriggerBot.cpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 19.02.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "TriggerBot.h"

CTriggerBot::CTriggerBot() {}

void CTriggerBot::CreateMove(CUserCmd *pCmd) {
    ButtonCode_t triggerKey = (ButtonCode_t)INIGET_INT("Improvements", "trigger_key");
    if(Interfaces::InputSystem()->IsButtonDown(triggerKey)) {
        C_CSPlayer* LocalPlayer = C_CSPlayer::GetLocalPlayer();
        if (!LocalPlayer || !LocalPlayer->IsValidLivePlayer()) {
            Reset();
            return;
        }

        C_BaseCombatWeapon* activeWeapon = LocalPlayer->GetActiveWeapon();
        if (!activeWeapon || activeWeapon->GetAmmo() == 0 || !CWeaponManager::IsValidWeapon(activeWeapon->GetWeaponEntityID())) {
            Reset();
            return;
        }

        if (!triggerTarget) {
            for (int eCount = 0; eCount < Interfaces::Engine()->GetMaxClients(); eCount++) {
                C_CSPlayer* PlayerEntity = C_CSPlayer::GetEntity(eCount);
                if (!PlayerEntity || !PlayerEntity->IsValidLivePlayer() || PlayerEntity->GetImmune()) {
                    continue;
                }

                if (PlayerEntity->GetTeamNum() == LocalPlayer->GetTeamNum()) {
                    continue;
                }

                if (!PlayerEntity->IsVisible(LocalPlayer)) {
                    continue;
                }

                if (!CanHitTarget(LocalPlayer, PlayerEntity, activeWeapon, pCmd)) {
                    continue;
                }

                triggerTarget = PlayerEntity;
                break;
            }

            return;
        }

        if (triggerTarget != nullptr) {
            bool delayExpired = false;
            if (INIGET_BOOL("Improvements", "trigger_delay")) {
                long currTime = Functions::GetTimeStamp();
                if (triggerTime == 0) {
                    triggerTime = currTime;
                }

                long triggerDelay = (long)Functions::SafeFloatToInt(INIGET_FLOAT("Improvements", "trigger_delay_value"));
                long currentDelay = currTime - triggerTime;
                if (currentDelay > triggerDelay) {
                    delayExpired = true;
                    triggerTime = 0;
                }
            } else {
                delayExpired = true;
            }

            if (delayExpired) {
                if (activeWeapon->GetWeaponEntityID() == EItemDefinitionIndex::weapon_revolver) {
                    pCmd->buttons |= IN_ATTACK2;
                } else {
                    pCmd->buttons |= IN_ATTACK;
                }
                Reset();
                return;
            }
        }
    } else {
        Reset();
        return;
    }
}

bool CTriggerBot::CanHitTarget(C_CSPlayer *LocalPlayer, C_CSPlayer *TargetEntity, C_BaseCombatWeapon *weapon, CUserCmd *pCmd) {
    if (!LocalPlayer || !LocalPlayer->IsValidLivePlayer()) {
        return false;
    }

    if (!TargetEntity || !TargetEntity->IsValidLivePlayer() || TargetEntity->GetImmune()) {
        return false;
    }

    if (!weapon || weapon->GetAmmo() == 0 || !CWeaponManager::IsValidWeapon(weapon->GetWeaponEntityID())) {
        return false;
    }

    Vector traceStart, traceEnd;

    QAngle viewAngles;
    Interfaces::Engine()->GetViewAngles(viewAngles);

    QAngle viewAnglesRCS = viewAngles + (LocalPlayer->AimPunch() * 2.f);

    CMath::AngleVectors(viewAnglesRCS, traceEnd);

    traceStart = LocalPlayer->GetEyePos();
    traceEnd = traceStart + (traceEnd * 8192.f);

    Ray_t ray;
    trace_t trace;
    CTraceFilter filter;
    filter.pSkip = LocalPlayer;

    ray.Init(traceStart, traceEnd);
    Interfaces::EngineTrace()->TraceRay(ray, MASK_SHOT, &filter, &trace);

    if (trace.m_pEnt && trace.m_pEnt == TargetEntity) {
        return true;
    }

    return false;
}

void CTriggerBot::Reset() {
    triggerTime = 0;
    triggerTarget = nullptr;
}
