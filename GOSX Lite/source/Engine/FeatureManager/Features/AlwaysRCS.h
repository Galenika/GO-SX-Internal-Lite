//
//  AlwaysRCS.hpp
//  GOSX Lite
//
//  Created by Andre Kalisch on 04.04.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#ifndef AlwaysRCS_h
#define AlwaysRCS_h

#include "SDK/SDK.h"
#include "SDK/CCSPlayer.h"
#include "Engine/WeaponManager/weapons.h"
#include "Engine/FeatureManager/Features/Aim.h"

class CAlwaysRCS {
public:
    CAlwaysRCS(std::shared_ptr<CAim> aimbot);
    void apply(CUserCmd* pCmd);
protected:
    QAngle lastRCSPunch = QAngle(0.0f, 0.0f, 0.0f);
    std::shared_ptr<CAim> aimInstance = nullptr;
};

#endif /* AlwaysRCS_h */
