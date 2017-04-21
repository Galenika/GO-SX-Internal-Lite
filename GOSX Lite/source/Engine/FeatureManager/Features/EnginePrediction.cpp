//
//  EnginePrediction.cpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 18.04.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "EnginePrediction.h"

CEnginePrediction* CEnginePrediction::instance = nullptr;

CEnginePrediction::CEnginePrediction() {}

std::map<int, const char*> CEnginePrediction::GetModeList() {
    std::map<int, const char*> select;

    select[0] = "0"; // simple
    select[1] = "1"; // full

    return select;
}

std::map<int, const char*> CEnginePrediction::GetModeValues() {
    std::map<int, const char*> values;

    values[0] = "simple"; // simple
    values[1] = "full"; // full

    return values;
}

void CEnginePrediction::Run(CUserCmd* cmd, C_BaseCombatWeapon* pWeapon, C_CSPlayer* pLocal) {
    if (!Interfaces::MoveHelper()) {
        return;
    }

    float backup = Interfaces::GlobalVars()->frametime;
    int oldFlags = *pLocal->GetFlags();

    Interfaces::GlobalVars()->frametime = Interfaces::GlobalVars()->interval_per_tick;

    pWeapon->UpdateAccuracyPenalty();

    CMoveData data;
    memset(&data, 0, sizeof(data));

    Interfaces::Prediction()->SetupMove(pLocal, cmd, Interfaces::MoveHelper(), &data);
    Interfaces::GameMovement()->ProcessMovement(pLocal, &data);
    Interfaces::Prediction()->FinishMove(pLocal, cmd, &data);

    Interfaces::GlobalVars()->frametime = backup;
    *pLocal->GetFlags() = oldFlags;
}

void CEnginePrediction::Start(CUserCmd* cmd) {
    if (!Interfaces::MoveHelper()) {
        return;
    }

    C_CSPlayer* LocalPlayer = C_CSPlayer::GetLocalPlayer();
    if (!LocalPlayer) {
        return;
    }

    C_BaseCombatWeapon* weapon = LocalPlayer->GetActiveWeapon();
    if (!weapon) {
        return;
    }

    oldPFlags = *LocalPlayer->GetFlags();

    m_flOldCurtime = Interfaces::GlobalVars()->curtime;
    m_flOldFrametime = Interfaces::GlobalVars()->frametime;

    Interfaces::GlobalVars()->curtime = LocalPlayer->GetTickBase() * Interfaces::GlobalVars()->interval_per_tick;
    Interfaces::GlobalVars()->frametime = Interfaces::GlobalVars()->interval_per_tick;

    weapon->UpdateAccuracyPenalty();

    CMoveData data;
    memset(&data, 0, sizeof(data));

    Interfaces::GameMovement()->StartTrackPredictionErrors(LocalPlayer);

    Interfaces::MoveHelper()->SetHost(LocalPlayer);
    Interfaces::Prediction()->SetupMove(LocalPlayer, cmd, Interfaces::MoveHelper(), &data);
    Interfaces::GameMovement()->ProcessMovement(LocalPlayer, &data);
    Interfaces::Prediction()->FinishMove(LocalPlayer, cmd, &data);
}

void CEnginePrediction::End() {
    if (!Interfaces::MoveHelper()) {
        return;
    }

    C_CSPlayer* LocalPlayer = C_CSPlayer::GetLocalPlayer();
    if (!LocalPlayer) {
        return;
    }

    Interfaces::GameMovement()->FinishTrackPredictionErrors(LocalPlayer);
    Interfaces::MoveHelper()->SetHost(0);

    *LocalPlayer->GetFlags() = oldPFlags;

    Interfaces::GlobalVars()->curtime = m_flOldCurtime;
    Interfaces::GlobalVars()->frametime = m_flOldFrametime;
}
