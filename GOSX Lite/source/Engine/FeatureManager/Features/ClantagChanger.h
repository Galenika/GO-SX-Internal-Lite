//
//  ClantagChanger.hpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 25.02.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#ifndef ClantagChanger_h
#define ClantagChanger_h

#include "SDK/SDK.h"
#include "SDK/CCSPlayer.h"
#include "SDK/Utils.h"

class CClantagChanger {
public:
    CClantagChanger();
    void apply();
    static std::map<int, const char*> GetList();
    static std::map<int, const char*> GetValues();
};

#endif /* ClantagChanger_h */
