//
//  Utils.cpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 27.02.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "Utils.h"

RankRevealAllFn                             Utils::m_fncRankRevealAll           = nullptr;
SendClanTagFn                               Utils::m_fncSetClanTag              = nullptr;

void Utils::RankRevealAll(float* input) {
    if(!m_fncRankRevealAll) {
        uintptr_t RankRevealAllPointer = CLIENT_POINTER(
            (Byte*)"\x48\x89\x85\x28\xFE\xFF\xFF\x48\xC7\x85\x30\xFE\xFF\xFF\x00\x00\x00\x00\x48\x8D\x05\x00\x00\x00\x00",
            "xxxxxxxxxxxxxx????xxx????",
            0x15
        ) + 0x4;
        m_fncRankRevealAll = reinterpret_cast<RankRevealAllFn>(RankRevealAllPointer);
    }

    m_fncRankRevealAll(input);
}

void Utils::SetClanTag(const char* tag, const char* name) {
    if(!m_fncSetClanTag) {
        uintptr_t SetClanTagPointer = ENGINE_POINTER(
            (Byte*)"\x48\x8D\x3D\x00\x00\x00\x00\x48\x89\xFE\xE8\x00\x00\x00\x00\xE9\x00\x00\x00\x00",
            "xxx????xxxx????x????",
            0xB
        ) + 0x4;
        m_fncSetClanTag = reinterpret_cast<SendClanTagFn>(SetClanTagPointer);
    }
    
    m_fncSetClanTag(tag, name);
}
