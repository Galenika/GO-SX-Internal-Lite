#pragma once

#ifndef createmove_h
#define createmove_h

#include "Engine/FeatureManager/Features/BunnyHop.h"
#include "Engine/FeatureManager/Features/TriggerBot.h"
#include "Engine/FeatureManager/Features/Aim.h"
#include "Engine/FeatureManager/Features/AlwaysRCS.h"
#include "Engine/FeatureManager/Features/AntiAim.h"
#include "Engine/FeatureManager/Features/EnginePrediction.h"

namespace FeatureManager {
    bool CreateMove(void* thisptr, float sample_input_frametime, CUserCmd* pCmd);
}

#endif // !createmove_h
