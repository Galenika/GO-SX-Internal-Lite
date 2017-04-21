//
//  IMoveHelper.h
//  GOSX Pro
//
//  Created by Andre Kalisch on 12.04.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#ifndef IMoveHelper_h
#define IMoveHelper_h

class IMoveHelper
{
public:
    void SetHost(C_BaseEntity* host) {
        typedef void (* oSetHost)(void*, C_BaseEntity*);
        return Internal::getvfunc<oSetHost>(this, 1)(this, host);
    }
};

#endif /* IMoveHelper_h */
