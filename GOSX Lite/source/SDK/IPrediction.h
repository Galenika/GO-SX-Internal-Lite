//
//  IPrediction.h
//  GOSX Pro
//
//  Created by Andre Kalisch on 12.04.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#ifndef IPrediction_h
#define IPrediction_h

class IPrediction {
public:

    void RunCommand(C_BaseEntity *player, CUserCmd *ucmd, IMoveHelper *moveHelper) {
        typedef void (*oRunCommand)(void*, C_BaseEntity*, CUserCmd*, IMoveHelper*);
        return Internal::getvfunc<oRunCommand>(this, 20)(this, player, ucmd, moveHelper);
    }

    void SetupMove(C_BaseEntity* player, CUserCmd* cmd, IMoveHelper* helper, CMoveData* move) {
        typedef void (* oSetupMove)(void*, C_BaseEntity*, CUserCmd*, IMoveHelper*, CMoveData*);
        return Internal::getvfunc<oSetupMove>(this, 21)(this, player, cmd, helper, move);
    }

    void FinishMove(C_BaseEntity* player, CUserCmd* cmd, CMoveData* move) {
        typedef void (* oFinishMove)(void*, C_BaseEntity*, CUserCmd*, CMoveData*);
        return Internal::getvfunc<oFinishMove>(this, 22)(this, player, cmd, move);
    }
};

#endif /* IPrediction_h */
