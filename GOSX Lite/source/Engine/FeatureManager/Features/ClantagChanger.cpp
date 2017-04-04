//
//  ClantagChanger.cpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 25.02.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "ClantagChanger.h"

CClantagChanger::CClantagChanger() {}

std::map<int, const char*> CClantagChanger::GetList() {
    std::map<int, const char*> clanTagSelect;

    clanTagSelect[0] = "";
    clanTagSelect[1] = "\n";
    clanTagSelect[2] = "[VALV\xE1\xB4\xB1]";

    return clanTagSelect;
}

std::map<int, const char*> CClantagChanger::GetValues() {
    std::map<int, const char*> clanTagValues;

    clanTagValues[0] = "-- Please Select --";
    clanTagValues[1] = "no-name";
    clanTagValues[2] = "Valve";

    return clanTagValues;
}

void CClantagChanger::apply() {
    C_CSPlayer* LocalPlayer = C_CSPlayer::GetLocalPlayer();
    if(!LocalPlayer || !LocalPlayer->IsValidLivePlayer()) {
        return;
    }

    if(INIGET_STRING("ClantagChanger", "tag") != LocalPlayer->GetClan()) {
        const char* tag = INIGET_STRING("ClantagChanger", "tag");
        const char* name = INIGET_STRING("ClantagChanger", "name");
        int preSelect = INIGET_INT("ClantagChanger", "pre_select");
        if(preSelect != 0) {
            int selected = preSelect;
            tag = CClantagChanger::GetList()[selected];
            name = CClantagChanger::GetValues()[selected];
        }

        Utils::SetClanTag(tag, name);
    }
}
