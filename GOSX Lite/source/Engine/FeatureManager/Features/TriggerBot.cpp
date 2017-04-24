//
//  TriggerBot.cpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 19.02.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "TriggerBot.h"

CTriggerBot::CTriggerBot() {}

void CTriggerBot::apply(CUserCmd *pCmd) {
    ButtonCode_t triggerKey = (ButtonCode_t)INIGET_INT("Improvements", "trigger_key");
    if (Interfaces::InputSystem()->IsButtonDown(triggerKey)) {
        C_CSPlayer* LocalPlayer = C_CSPlayer::GetLocalPlayer();
        if (!LocalPlayer || !LocalPlayer->IsValidLivePlayer()) {
            return;
        }

        C_BaseCombatWeapon* active_weapon = LocalPlayer->GetActiveWeapon();
        if (!active_weapon || active_weapon->GetAmmo() == 0 || !CWeaponManager::IsValidWeapon(active_weapon->GetWeaponEntityID())) {
            return;
        }

        Vector traceStart, traceEnd;

        QAngle viewAngles;
        Interfaces::Engine()->GetViewAngles(viewAngles);
        QAngle viewAngles_rcs = viewAngles + (LocalPlayer->AimPunch() * 2.0f);

        CMath::AngleVectors(viewAngles_rcs, traceEnd);

        traceStart = LocalPlayer->GetEyePos();
        traceEnd = traceStart + (traceEnd * 8192.0f);

        Ray_t ray;
        trace_t trace;
        CTraceFilter filter;
        filter.pSkip = LocalPlayer;

        ray.Init(traceStart, traceEnd);
        Interfaces::EngineTrace()->TraceRay(ray, MASK_SHOT, &filter, &trace);

        if (trace.allsolid || trace.startsolid) {
            return;
        }

        C_CSPlayer* player = (C_CSPlayer*)trace.m_pEnt;
        if (!player || !player->IsValidLivePlayer() || player->GetImmune()) {
            return;
        }

        if (player->GetClientClass()->m_ClassID != EClassIds::CCSPlayer) {
            return;
        }

        if (LocalPlayer->GetTeamNum() == player->GetTeamNum()) {
            return;
        }

        if (INIGET_BOOL("Improvements", "trigger_delay")) {
            float currTime = Interfaces::GlobalVars()->curtime;
            if (triggerTime == 0.f) {
                triggerTime = currTime;
            }

            float triggerDelay = (float)(INIGET_INT("Improvements", "trigger_delay_value") / 1000);
            if ((currTime - triggerTime) < triggerDelay) {
                return;
            }

            triggerTime = currTime;
        }

        if (active_weapon->NextPrimaryAttack() > Interfaces::GlobalVars()->curtime) {
            if (active_weapon->GetWeaponEntityID() == weapon_revolver) {
                pCmd->buttons &= ~IN_ATTACK2;
            } else {
                pCmd->buttons &= ~IN_ATTACK;
            }
        } else {
            if (active_weapon->GetWeaponEntityID() == weapon_revolver) {
                pCmd->buttons |= IN_ATTACK2;
            } else {
                pCmd->buttons |= IN_ATTACK;
            }
        }
    } else {
        if (triggerTime > 0.f) {
            triggerTime = 0.f;
        }
    }
}
