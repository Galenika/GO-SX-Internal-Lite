#include "CreateMove.h"
#include "SDK/Utils.h"

namespace FeatureManager {
    std::shared_ptr<CBunnyHop> g_cBunnyHop = nullptr;
    std::shared_ptr<CTriggerBot> g_cTriggerBot = nullptr;
    std::shared_ptr<CAim> g_cAim = nullptr;
    std::shared_ptr<CAlwaysRCS> g_cAlwaysRCS = nullptr;
    std::shared_ptr<CAntiAim> g_cAntiAim = nullptr;

    bool CreateMove(void* thisptr, float sample_input_frametime, CUserCmd* pCmd) {
        bool bRet = true;

        if (INIGET_BOOL("Improvements", "bunnyhop")) {
            if (!g_cBunnyHop) {
                g_cBunnyHop = std::make_unique<CBunnyHop>();
            }
            g_cBunnyHop->run(pCmd);
        }

        if (INIGET_BOOL("Improvements", "rankreveal")) {
            if (pCmd->buttons & IN_SCORE) {
                float input[3] = { 0.f };
                Utils::RankRevealAll(input);
            }
        }

        if (INIGET_BOOL("AimHelper", "enabled")) {
            if (!g_cAim) {
                g_cAim = std::make_unique<CAim>();
            }
            bool EnginePredictActive = INIGET_BOOL("Rage", "enabled") &&
                                       INIGET_BOOL("Rage", "engine_predict") &&
                                       INIGET_INT("Rage", "engine_predict_mode") == 1;

            if (EnginePredictActive) {
                CEnginePrediction::Instance()->Start(pCmd);
            }

            g_cAim->CreateMove(pCmd);

            if (EnginePredictActive) {
                CEnginePrediction::Instance()->End();
            }
        }

        if (INIGET_BOOL("Rage", "enabled") && INIGET_BOOL("Rage", "anti_aim")) {
            if (!g_cAntiAim) {
                g_cAntiAim = CAntiAim::Instance();
            }

            g_cAntiAim->CreateMove(pCmd);
        }

        if (INIGET_BOOL("Improvements", "always_rcs")) {
            if(!g_cAlwaysRCS) {
                if (!g_cAim) {
                    g_cAim = std::make_unique<CAim>();
                }
                g_cAlwaysRCS = std::make_unique<CAlwaysRCS>(g_cAim);
            }
            g_cAlwaysRCS->apply(pCmd);
        }

        if (INIGET_BOOL("Improvements", "triggerbot")) {
            if (!g_cTriggerBot) {
                g_cTriggerBot = std::make_unique<CTriggerBot>();
            }
            g_cTriggerBot->CreateMove(pCmd);
        }

        return bRet;
    }
}
