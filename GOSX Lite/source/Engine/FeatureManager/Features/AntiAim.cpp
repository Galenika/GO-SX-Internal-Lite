//
//  AntiAim.cpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 19.04.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "AntiAim.h"

std::shared_ptr<CAntiAim> CAntiAim::instance = nullptr;

CAntiAim::CAntiAim() {}

std::map<int, const char*> CAntiAim::GetPitchList() {
    std::map<int, const char*> select;

    select[0] = "0"; // Emotion
    select[1] = "1"; // Up
    select[2] = "2"; // Down
    select[3] = "3"; // Custom

    return select;
}
std::map<int, const char*> CAntiAim::GetPitchValues() {
    std::map<int, const char*> values;

    values[0] = "Emotion";
    values[1] = "Up";
    values[2] = "Down";
    values[3] = "Custom";

    return values;
}
std::map<int, const char*> CAntiAim::GetYawList() {
    std::map<int, const char*> select;

    select[0] = "0"; // Backwards
    select[1] = "1"; // Left
    select[2] = "2"; // Right
    select[3] = "3"; // Custom

    return select;
}

std::map<int, const char*> CAntiAim::GetYawValues() {
    std::map<int, const char*> values;

    values[0] = "Backwards";
    values[1] = "Left";
    values[2] = "Right";
    values[3] = "Custom";

    return values;
}

void CAntiAim::AAPitch(QAngle& angle) {
    EAntiAimPitch AntiaimTypePitch = (EAntiAimPitch)INIGET_INT("Rage", "antiaim_pitch");

    switch (AntiaimTypePitch) {
        case EAntiAimPitch::Emotion:
            angle.x = 82.0f;
            break;
        case EAntiAimPitch::Down:
            angle.x = 89.0f;
            break;
        case EAntiAimPitch::Up:
            angle.x = -89.0f;
            break;
        case EAntiAimPitch::CustomPitch:
            angle.x = 89.f - INIGET_FLOAT("Rage", "antiaim_custom_pitch");
            break;
        default:
            angle.x -= 0.0f;
            break;
    }
}

void CAntiAim::AAYaw(QAngle& angle) {
    EAntiAimYaw AntiaimTypeYaw = (EAntiAimYaw)INIGET_INT("Rage", "antiaim_yaw");

    switch (AntiaimTypeYaw) {
        case EAntiAimYaw::Backwards:
            angle.y -= 180.0f;
            break;
        case EAntiAimYaw::Left:
            angle.y += 90.0f;
            break;
        case EAntiAimYaw::Right:
            angle.y -= 90.0f;
            break;
        case EAntiAimYaw::CustomYaw:
            angle.y = 180.f - INIGET_FLOAT("Rage", "antiaim_custom_yaw");
            break;
        default:
            angle.y -= 0.0f;
            break;
    }
}

void CAntiAim::CreateMove(CUserCmd* pCmd) {
    C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();
    if (!pLocal || !pLocal->IsAlive()) {
        return;
    }

    C_BaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon();
    if (!pWeapon) {
        return;
    }

    QAngle oldAngle = pCmd->viewangles;
    float oldForward = pCmd->forwardmove;
    float oldSideMove = pCmd->sidemove;

    QAngle angle = pCmd->viewangles;

    if (
        pCmd->buttons & IN_USE ||
        pCmd->buttons & IN_ATTACK ||
        (pCmd->buttons & IN_ATTACK2 && pWeapon->GetWeaponEntityID() == EItemDefinitionIndex::weapon_revolver)
    ) {
        return;
    }

    if (pLocal->GetMoveType() == MOVETYPE_LADDER || pLocal->GetMoveType() == MOVETYPE_NOCLIP) {
        return;
    }

    if (pLocal->IsAlive() && pWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE) {
        return;
    }

    if (pLocal->IsAlive() && pWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_GRENADE) {
        return;
    }

    if (
        INIGET_BOOL("Improvements", "triggerbot") &&
        Interfaces::InputSystem()->IsButtonDown(
            (ButtonCode_t)INIGET_INT("Improvements", "trigger_key")
        )
    ) {
        return;
    }

    AAYaw(angle);
    CMath::NormalizeAngles(angle);

    AAPitch(angle);
    CMath::NormalizeAngles(angle);
    
    CMath::ClampAngle(angle);

    pCmd->viewangles = angle;
    
    CMath::CorrectMovement(oldAngle, pCmd, oldForward, oldSideMove);
}
