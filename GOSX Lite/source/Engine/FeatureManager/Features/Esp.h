//
//  Esp.hpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 20.02.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#ifndef Esp_h
#define Esp_h

#include "SDK/SDK.h"
#include "Engine/DrawManager/draw.h"
#include "SDK/Utils.h"

class CEsp {
public:
    CEsp(std::shared_ptr<CDrawings> drawMngr);
    void DrawESP();
    void DrawSmokeESP();
private:
    std::shared_ptr<CDrawings> DrawManager;
};

#endif /* Esp_h */
