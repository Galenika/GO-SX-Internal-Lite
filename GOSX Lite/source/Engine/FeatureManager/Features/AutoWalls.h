//
//  AutoWalls.hpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 18.04.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#ifndef AutoWalls_h
#define AutoWalls_h

#include "SDK/SDK.h"
#include "SDK/Definitions.h"
#include "SDK/CCSPlayer.h"

struct FireBulletData {
    Vector          src;
    trace_t         enter_trace;
    Vector          direction;
    CTraceFilter    filter;
    float           trace_length;
    float           trace_length_remaining;
    float           current_damage;
    int             penetrate_count;
};

class CAutoWalls {
public:
    static std::shared_ptr<CAutoWalls> Instance() {
        if (!instance) {
            instance = std::make_unique<CAutoWalls>();
        }

        return instance;
    }
    CAutoWalls();
    void TraceLine(Vector vecAbsStart, Vector vecAbsEnd, unsigned int mask, C_CSPlayer* ignore, trace_t* ptr);
    float GetHitgroupDamageMultiplier(int iHitGroup);
    bool TraceToExit(Vector &end, trace_t *enter_trace, Vector start, Vector dir, trace_t *exit_trace);
    void ScaleDamage(int hitgroup, C_CSPlayer* enemy, float weapon_armor_ratio, float & current_damage);
    bool HandleBulletPenetration(CCSWeaponInfo* weaponInfo, FireBulletData &data);
    bool SimulateFireBullet(C_CSPlayer* pLocal, C_BaseCombatWeapon* pWeapon, FireBulletData &data);
    float GetDamage(C_CSPlayer* pLocal, C_CSPlayer* pEntity, int HitboxIndex);
    float GetDamage(C_CSPlayer* pLocal, Vector HitboxPosition);
private:
    static std::shared_ptr<CAutoWalls> instance;
};

#endif /* AutoWalls_h */
