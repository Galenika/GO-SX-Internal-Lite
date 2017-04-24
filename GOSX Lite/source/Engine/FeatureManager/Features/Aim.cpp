//
//  Aim.cpp
//  GOSX Lite
//
//  Created by Andre Kalisch on 30.03.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "Aim.h"
#include "AutoWalls.h"
#include "EnginePrediction.h"

CAim::CAim() {}

std::map<int, const char*> CAim::GetBoneList() {
    std::map<int, const char*> select;

    select[0] = "0"; // HITBOX_HEAD
    select[1] = "1"; // HITBOX_NECK
    select[2] = "2"; // HITBOX_LOWER_NECK
    select[3] = "3"; // HITBOX_PELVIS
    select[4] = "4"; // HITBOX_BODY
    select[5] = "5"; // HITBOX_THORAX
    select[6] = "6"; // HITBOX_CHEST
    select[7] = "7"; // HITBOX_UPPER_CHEST

    return select;
}

std::map<int, const char*> CAim::GetBoneValues() {
    std::map<int, const char*> values;

    values[0] = "HEAD";
    values[1] = "NECK";
    values[2] = "LOWER NECK";
    values[3] = "PELVIS";
    values[4] = "BODY";
    values[5] = "THORAX";
    values[6] = "CHEST";
    values[7] = "UPPER CHEST";

    return values;
}

void CAim::CreateMove(CUserCmd *pCmd) {
    C_CSPlayer* LocalPlayer = C_CSPlayer::GetLocalPlayer();
    if (!LocalPlayer || !LocalPlayer->IsValidLivePlayer()) {
        Reset();
        return;
    }

    C_BaseCombatWeapon* currentWeapon = LocalPlayer->GetActiveWeapon();
    if (!currentWeapon) {
        Reset();
        return;
    }

    int currentWeaponID = currentWeapon->GetWeaponEntityID();
    if (
        CWeaponManager::isC4(currentWeaponID) ||
        CWeaponManager::isKnife(currentWeaponID) ||
        CWeaponManager::isGrenade(currentWeaponID) ||
        CWeaponManager::isNonAimWeapon(currentWeaponID)
    ) {
        Reset();
        return;
    }

    if (!INIGET_BOOL("Rage", "enabled")) {
        if (
            (!Interfaces::InputSystem()->IsButtonDown(MOUSE_LEFT) && currentWeaponID != EItemDefinitionIndex::weapon_revolver) ||
            (!Interfaces::InputSystem()->IsButtonDown(MOUSE_RIGHT) && currentWeaponID == EItemDefinitionIndex::weapon_revolver)
        ) {
            Reset();
            return;
        }
    } else {
        if (!INIGET_BOOL("Rage", "auto_shoot")) {
            if (
                (!Interfaces::InputSystem()->IsButtonDown(MOUSE_LEFT) && currentWeaponID != EItemDefinitionIndex::weapon_revolver) ||
                (!Interfaces::InputSystem()->IsButtonDown(MOUSE_RIGHT) && currentWeaponID == EItemDefinitionIndex::weapon_revolver)
            ) {
                Reset();
                return;
            }
        }
    }

    C_CSPlayer* TargetEntity = nullptr;
    if (!LockedTargetEntity) {
        TargetEntity = FindTarget(LocalPlayer);
        if (!TargetEntity || !TargetEntity->IsValidLivePlayer() || TargetEntity->GetImmune()) {
            Reset();
            return;
        }
        LockedTargetEntity = TargetEntity;
    } else {
        TargetEntity = LockedTargetEntity;
    }

    if (!TargetEntity || !TargetEntity->IsValidLivePlayer() || TargetEntity->GetImmune()) {
        Reset();
        return;
    }

    if (INIGET_BOOL("Rage", "enabled") && INIGET_BOOL("Rage", "engine_predict") && INIGET_INT("Rage", "engine_predict_mode") == 0) {
        CEnginePrediction::Instance()->Run(pCmd, currentWeapon, LocalPlayer);
    }

    StartAim(LocalPlayer, TargetEntity, pCmd);
}

C_CSPlayer* CAim::FindTarget(C_CSPlayer* LocalPlayer) {
    float fov = INIGET_FLOAT("AimHelper", "aim_fov");
    if(INIGET_BOOL("Rage", "enabled")) {
        fov *= INIGET_FLOAT("Rage", "fov_multiplier");
    }
    C_CSPlayer* TargetEntity = nullptr;

    Vector LocalPlayerEyePosition = LocalPlayer->GetEyePos();
    Vector LocalPlayerViewAngle = LocalPlayer->GetViewAngle();

    for(int i = 1; i < Interfaces::Engine()->GetMaxClients(); i++) {
        C_CSPlayer* PossibleTarget = C_CSPlayer::GetEntity(i);
        if (!PossibleTarget || !PossibleTarget->IsValidLivePlayer() || PossibleTarget->GetImmune()) {
            continue;
        }

        if (INIGET_BOOL("AimHelper", "team_check") && LocalPlayer->GetTeamNum() == PossibleTarget->GetTeamNum()) {
            continue;
        }

        int aimHitbox;
        if (INIGET_BOOL("Rage", "enabled") && INIGET_BOOL("Rage", "hit_scan")) {
            aimHitbox = MakeHitscan(LocalPlayer, PossibleTarget);
        } else {
            aimHitbox = INIGET_INT("AimHelper", "aim_hitbox");
        }

        Vector PossibleTargetHitbox;
        if (INIGET_BOOL("Rage", "enabled") && INIGET_BOOL("Rage", "engine_predict")) {
            PossibleTargetHitbox = PossibleTarget->GetPredictedPosition(aimHitbox);
        } else {
            PossibleTargetHitbox = PossibleTarget->GetHitboxPosition(aimHitbox);
        }

        bool IsEntityVisible = PossibleTarget->IsVisible(LocalPlayer, PossibleTargetHitbox);
        if (!IsEntityVisible) {
            if (INIGET_BOOL("Rage", "enabled")) {
                if (INIGET_BOOL("Rage", "auto_wall")) {
                    float AutoWallDamage = CAutoWalls::Instance()->GetDamage(LocalPlayer, PossibleTargetHitbox);
                    if (AutoWallDamage < INIGET_FLOAT("Rage", "autowall_min_damage")) {
                        continue;
                    }
                } else {
                    continue;
                }
            } else {
                continue;
            }
        }

        float FieldOfViewToTarget = CMath::GetFov(LocalPlayerViewAngle, LocalPlayerEyePosition, PossibleTargetHitbox);

        if (FieldOfViewToTarget < fov) {
            TargetEntity = PossibleTarget;
            fov = FieldOfViewToTarget;
        }
    }

    if (!TargetEntity || !TargetEntity->IsValidLivePlayer()) {
        return nullptr;
    }

    return TargetEntity;
}

void CAim::StartAim(C_CSPlayer* LocalPlayer, C_CSPlayer* AimTarget, CUserCmd* pCmd) {
    Vector AimAngle, OldAngle;
    Interfaces::Engine()->GetViewAngles(OldAngle);
    float oldForward = pCmd->forwardmove;
    float oldSideMove = pCmd->sidemove;

    int LockedBone;
    if (LockedEntityBone == -5) {
        if (INIGET_BOOL("Rage", "enabled") && INIGET_BOOL("Rage", "hit_scan")) {
            LockedBone = MakeHitscan(LocalPlayer, AimTarget);
        } else {
            LockedBone = INIGET_INT("AimHelper", "aim_hitbox");
        }
        LockedEntityBone = LockedBone;
    } else {
        LockedBone = LockedEntityBone;
    }

    Vector TargetHitbox;
    if (INIGET_BOOL("Rage", "enabled") && INIGET_BOOL("Rage", "engine_predict")) {
        TargetHitbox = AimTarget->GetPredictedPosition(LockedBone);
    } else {
        TargetHitbox = AimTarget->GetHitboxPosition(LockedBone);
    }

    bool IsEntityVisible = AimTarget->IsVisible(LocalPlayer, TargetHitbox);
    if (!IsEntityVisible) {
        if (INIGET_BOOL("Rage", "enabled")) {
            if (INIGET_BOOL("Rage", "auto_wall")) {
                float AutoWallDamage = CAutoWalls::Instance()->GetDamage(LocalPlayer, TargetHitbox);
                if (AutoWallDamage < INIGET_FLOAT("Rage", "autowall_min_damage")) {
                    Reset();
                    return;
                }
            } else {
                Reset();
                return;
            }
        } else {
            Reset();
            return;
        }
    }

    Vector dir = LocalPlayer->GetEyePos() - TargetHitbox;
    
    CMath::VectorNormalize(dir);
    CMath::VectorAngles(dir, AimAngle);

    RecoilControl(AimAngle, LocalPlayer, AimTarget, pCmd);
    if (!INIGET_BOOL("Rage", "enabled")) {
        Smooth(AimAngle);
    } else {
        AutoCrouch(LocalPlayer, AimTarget, pCmd);
        AutoStop(LocalPlayer, AimTarget, pCmd);
    }

    oldForward = pCmd->forwardmove;
    oldSideMove = pCmd->sidemove;

    if (!AimTarget->IsValidLivePlayer()) {
        Reset();
        return;
    }

    if (AimAngle != OldAngle) {
        CMath::NormalizeAngles(AimAngle);
        CMath::ClampAngle(AimAngle);
        if(AimAngle.IsValid()) {
            if (!HasValidTargetAndIsAiming) {
                HasValidTargetAndIsAiming = true;
            }

            if(INIGET_BOOL("Rage", "enabled") && INIGET_BOOL("Rage", "silent_aim")) {
                pCmd->viewangles = AimAngle;
            } else {
                CMath::CorrectMovement(OldAngle, pCmd, oldForward, oldSideMove);
                Interfaces::Engine()->SetViewAngles(AimAngle);
            }

            if (INIGET_BOOL("Rage", "enabled")) {
                C_BaseCombatWeapon* activeWeapon = LocalPlayer->GetActiveWeapon();
                AutoShoot(AimTarget, activeWeapon, pCmd);
            }
        }
    }
}

void CAim::RecoilControl(QAngle& angle, C_CSPlayer* LocalPlayer, C_CSPlayer* player, CUserCmd* cmd, bool inAttack) {
    if (!INIGET_BOOL("Rage", "enabled")) {
        if (!INIGET_BOOL("AimHelper", "aim_rcs")) {
            return;
        }
    }

    if (!HasTarget()) {
        return;
    }

    C_BaseCombatWeapon* currentWeapon = LocalPlayer->GetActiveWeapon();
    if (!currentWeapon) {
        return;
    }

    int currentWeaponID = currentWeapon->GetWeaponEntityID();
    QAngle CurrentPunch = LocalPlayer->AimPunch();
    if (CurrentPunch.x != 0.f || CurrentPunch.y != 0.f) {
        if (!INIGET_BOOL("Rage", "enabled")) {
            if (CWeaponManager::isRCSWeapon(currentWeaponID)) {
                angle.x -= CurrentPunch.x * INIGET_FLOAT("AimHelper", "aim_rcs_level");
                angle.y -= CurrentPunch.y * INIGET_FLOAT("AimHelper", "aim_rcs_level");
            }
        } else {
            angle.x -= CurrentPunch.x * INIGET_FLOAT("AimHelper", "aim_rcs_level");
            angle.y -= CurrentPunch.y * INIGET_FLOAT("AimHelper", "aim_rcs_level");
        }
    }
}

void CAim::Reset() {
    LockedEntityBone = -5;
    LockedTargetEntity = nullptr;
    HasValidTargetAndIsAiming = false;
}

bool CAim::HasTarget() {
    return HasValidTargetAndIsAiming &&
           LockedTargetEntity != nullptr &&
           LockedEntityBone != -5;
}

void CAim::AutoCrouch(C_CSPlayer* LocalPlayer, C_CSPlayer* EntityPlayer, CUserCmd* pCmd, bool inAttack) {
    if (!INIGET_BOOL("Rage", "auto_crouch")) {
        return;
    }

    if (!HasTarget()) {
        return;
    }

    if (!LocalPlayer || !LocalPlayer->IsValidLivePlayer()) {
        return;
    }

    C_BaseCombatWeapon* currentWeapon = LocalPlayer->GetActiveWeapon();
    if (
        !currentWeapon ||
        !CWeaponManager::isRCSWeapon(currentWeapon->GetWeaponEntityID())
    ) {
        return;
    }

    if (
        (LocalPlayer->GetShotsFired() > 0 && (pCmd->buttons & IN_ATTACK)) ||
        INIGET_BOOL("Rage", "enabled")
    ) {
        if (
            Interfaces::InputSystem()->IsButtonDown(KEY_LSHIFT) ||
            CMath::VecDist(LocalPlayer->GetEyePos(), EntityPlayer->GetEyePos()) > 550.0f ||
            INIGET_BOOL("Rage", "enabled")
        ) {
            pCmd->buttons |= IN_DUCK;
        }
    }
}

void CAim::AutoStop(C_CSPlayer* LocalPlayer, C_CSPlayer* EntityPlayer, CUserCmd* cmd) {
    if (!INIGET_BOOL("Rage", "auto_stop")) {
        return;
    }

    if (!HasTarget()) {
        return;
    }

    if (
        (LocalPlayer->GetShotsFired() > 0 && (cmd->buttons & IN_ATTACK)) ||
        INIGET_BOOL("Rage", "enabled")
    ) {
        if (
            Interfaces::InputSystem()->IsButtonDown(KEY_LSHIFT) ||
            CMath::VecDist(LocalPlayer->GetEyePos(), EntityPlayer->GetEyePos()) > 550.0f ||
            INIGET_BOOL("Rage", "enabled")
        ) {
            cmd->forwardmove = 0.f;
            cmd->sidemove = 0.f;
            cmd->upmove = 0.f;
        }
    }
}

void CAim::AutoShoot(C_CSPlayer* player, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd) {
    if (!INIGET_BOOL("Rage", "auto_shoot")) {
        return;
    }

    if (!player || !activeWeapon || activeWeapon->GetAmmo() == 0) {
        return;
    }

    if (!HasTarget()) {
        return;
    }

    if (!CWeaponManager::IsValidWeapon(activeWeapon->GetWeaponEntityID())) {
        return;
    }

    if (cmd->buttons & IN_USE) {
        return;
    }

    C_CSPlayer* localplayer = C_CSPlayer::GetLocalPlayer();
    float nextPrimaryAttack = activeWeapon->NextPrimaryAttack();

    if (nextPrimaryAttack > Interfaces::GlobalVars()->curtime) {
        if (!CWeaponManager::isRCSWeapon(activeWeapon->GetWeaponEntityID())) {
            if (activeWeapon->GetWeaponEntityID() == EItemDefinitionIndex::weapon_revolver) {
                cmd->buttons &= ~IN_ATTACK2;
            } else {
                cmd->buttons &= ~IN_ATTACK;
            }
        }
    } else {
        if (INIGET_BOOL("Rage", "auto_scope") && activeWeapon->GetCSWpnData()->GetZoomLevels() > 0 && !localplayer->IsScoped()) {
            cmd->buttons |= IN_ATTACK2;
        } else if (activeWeapon->GetWeaponEntityID() == EItemDefinitionIndex::weapon_revolver) {
            cmd->buttons |= IN_ATTACK2;
        } else {
            cmd->buttons |= IN_ATTACK;
        }
    }
}

void CAim::Smooth(QAngle& angle)
{
    if (!INIGET_BOOL("AimHelper", "aim_smooth")) {
        return;
    }

    QAngle viewAngles = QAngle(0.f, 0.f, 0.f);
    Interfaces::Engine()->GetViewAngles(viewAngles);

    QAngle delta = angle - viewAngles;
    CMath::NormalizeAngles(delta);

    float smooth = powf(INIGET_FLOAT("AimHelper", "aim_smoothing"), 0.4f);
    smooth = std::min(0.99f, smooth);

    QAngle toChange = delta - delta * smooth;

    angle = viewAngles + toChange;
}

int CAim::MakeHitscan(C_CSPlayer* pLocal, C_CSPlayer* pEntity) {
    std::vector<int> hitboxes;
    hitboxes.push_back((int)PlayerBones::Head);
    hitboxes.push_back((int)PlayerBones::Neck);
    hitboxes.push_back((int)PlayerBones::Neck_Lower);
    hitboxes.push_back((int)PlayerBones::Pelvis);
    hitboxes.push_back((int)PlayerBones::Stomach);
    hitboxes.push_back((int)PlayerBones::Lower_Chest);
    hitboxes.push_back((int)PlayerBones::Chest);
    hitboxes.push_back((int)PlayerBones::Upper_Chest);
    hitboxes.push_back((int)PlayerBones::Right_Thigh);
    hitboxes.push_back((int)PlayerBones::Left_Thigh);
    hitboxes.push_back((int)PlayerBones::Right_Shin);
    hitboxes.push_back((int)PlayerBones::Left_Shin);
    hitboxes.push_back((int)PlayerBones::Right_Foot);
    hitboxes.push_back((int)PlayerBones::Left_Foot);

    for (auto hit : hitboxes) {
        float flDmg = CAutoWalls::Instance()->GetDamage(pLocal, pEntity->GetPredictedPosition(hit));
        if (flDmg > 0.f) {
            return hit;
        }
    }

    return (int)PlayerBones::Head;
}
