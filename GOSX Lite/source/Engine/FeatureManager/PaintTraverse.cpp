//
//  PaintTraverse.cpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 15.02.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "PaintTraverse.h"

bool toggle = false;

namespace FeatureManager
{
    std::shared_ptr<CEsp> g_cEsp = nullptr;
    void PaintTraverse(void* thisptr, VPANEL vguiPanel, bool forceRepaint, bool allowForce)
    {
        std::shared_ptr<CDrawings> drawManager = CDrawings::Instance();
        std::string szPanelName = Interfaces::Panel()->GetName(vguiPanel);
        if(INIGET_BOOL("Drawing", "enabled")) {
            if (szPanelName.find("FocusOverlayPanel") != std::string::npos) {
                drawManager->DrawShadowString(1, 35, 5, Color(255, 255, 255), false, "GO:SX Lite");
                if(!CHackMenu::Instance()->IsMenuOpen()) {
                    if (Interfaces::Engine()->IsInGame()) {
                        if (!g_cEsp) {
                            g_cEsp = std::make_unique<CEsp>(drawManager);
                        }

                        if(INIGET_BOOL("Drawing", "playeresp")) {
                            g_cEsp->DrawESP();
                        }

                        if (!INIGET_BOOL("Drawing", "playeresp") && INIGET_BOOL("Drawing", "smoke_esp")) {
                            g_cEsp->DrawSmokeESP();
                        }
                    }
                }
            }
        }
    }
}
