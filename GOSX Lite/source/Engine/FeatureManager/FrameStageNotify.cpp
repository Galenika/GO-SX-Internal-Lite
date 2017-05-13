//
//  FrameStageNotify.cpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 19.02.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "FrameStageNotify.h"

namespace FeatureManager {
    std::shared_ptr<CFlashReducer> g_cFlashReducer = nullptr;
    std::shared_ptr<CSkinChanger> g_cSkinChanger = nullptr;
    std::shared_ptr<CClantagChanger> g_cClantagChanger = nullptr;
    
    void FrameStageNotify(ClientFrameStage_t stage) {
        if(Interfaces::Engine()->IsInGame()) {
            if(stage == ClientFrameStage_t::FRAME_START) {
                if(INIGET_BOOL("Improvements", "no_flash")) {
                    if(!g_cFlashReducer) {
                        g_cFlashReducer = std::make_unique<CFlashReducer>();
                    }
                    g_cFlashReducer->apply();
                }

                if(INIGET_BOOL("ClantagChanger", "enabled")) {
                    if(!g_cClantagChanger) {
                        g_cClantagChanger = std::make_unique<CClantagChanger>();
                    }
                    g_cClantagChanger->apply();
                }
            }

            if(stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START){
                if(INIGET_BOOL("Improvements", "skin_changer")) {
                    if(!g_cSkinChanger) {
                        g_cSkinChanger = std::make_unique<CSkinChanger>();
                    }

                    g_cSkinChanger->apply(stage);
                }
            }
        }
    }
}
