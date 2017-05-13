//
//  AutoWalls.cpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 18.04.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "AutoWalls.h"

std::shared_ptr<CAutoWalls> CAutoWalls::instance = nullptr;

CAutoWalls::CAutoWalls() {}

void CAutoWalls::TraceLine(Vector vecAbsStart, Vector vecAbsEnd, unsigned int mask, C_CSPlayer* ignore, trace_t* ptr) {
    Ray_t ray;
    ray.Init(vecAbsStart, vecAbsEnd);
    CTraceFilter filter;
    filter.pSkip = ignore;

    Interfaces::EngineTrace()->TraceRay(ray, mask, &filter, ptr);
}

float CAutoWalls::GetHitgroupDamageMultiplier(int iHitGroup) {
    switch (iHitGroup) {
        case HITGROUP_GENERIC:
            return 1.0f;

        case HITGROUP_HEAD:
            return 4.0f;

        case HITGROUP_CHEST:
            return 1.0f;

        case HITGROUP_STOMACH:
            return 1.25f;

        case HITGROUP_LEFTARM:
        case HITGROUP_RIGHTARM:
            return 1.0f;

        case HITGROUP_LEFTLEG:
        case HITGROUP_RIGHTLEG:
            return 0.75f;

        default:
            return 1.0f;
    }
}

bool CAutoWalls::TraceToExit(Vector &end, trace_t *enter_trace, Vector start, Vector dir, trace_t *exit_trace) {
    float distance = 0.0f;

    while (distance <= 90.0f) {
        distance += 4.0f;
        end = start + dir * distance;

        auto point_contents = Interfaces::EngineTrace()->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX, NULL);

        if (point_contents & MASK_SHOT_HULL && (!(point_contents & CONTENTS_HITBOX))) {
            continue;
        }

        auto new_end = end - (dir * 4.0f);

        TraceLine(end, new_end, 0x4600400B, 0, exit_trace);

        // bool allsolid; @ TR + 54
        // bool startsolid; @ TR + 55
        if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX) {
            TraceLine(end, start, 0x600400B, (C_CSPlayer*)exit_trace->m_pEnt, exit_trace);

            if ((exit_trace->fraction < 1.0f || exit_trace->allsolid) && !exit_trace->startsolid) {
                end = exit_trace->endpos;
                return true;
            }
            continue;
        }

        if (!(exit_trace->fraction < 1.0 || exit_trace->allsolid || exit_trace->startsolid ) || exit_trace->startsolid) {
            if (exit_trace->m_pEnt) {
                if (enter_trace->m_pEnt && enter_trace->m_pEnt == Interfaces::EntityList()->GetClientEntity(0)) {
                    return true;
                }
            }
            continue;
        }

        // checking for SURF_NODRAW perhaps
        if (((exit_trace->surface.flags >> 7) & 1) && !((enter_trace->surface.flags >> 7) & 1)) {
            continue;
        }

        if (exit_trace->plane.normal.Dot( dir ) <= 1.0f) {
            auto fraction = exit_trace->fraction * 4.0f;
            end = end - ( dir * fraction );
            return true;
        }
    }
    return false;
}

void CAutoWalls::ScaleDamage(int hitgroup, C_CSPlayer* enemy, float weapon_armor_ratio, float & current_damage) {
    current_damage *= GetHitgroupDamageMultiplier(hitgroup);

    int armor = enemy->GetArmor();

    if (armor > 0) {
        if (hitgroup == (int)HitGroups::HITGROUP_HEAD) {
            if (enemy->HasHelmet()) {
                current_damage *= (weapon_armor_ratio);
            }
        } else {
            current_damage *= (weapon_armor_ratio);
        }
    }
}

bool CAutoWalls::HandleBulletPenetration(CCSWeaponInfo* weaponInfo, FireBulletData &data) {
    surfacedata_t *enter_surface_data = Interfaces::Physics()->GetSurfaceData(data.enter_trace.surface.surfaceProps);
    int enter_material = enter_surface_data->game.material;
    float enter_surf_penetration_mod = *(float*)((uintptr_t)enter_surface_data + 76);

    data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
    data.current_damage *= pow((weaponInfo->GetRangeModifier()), (data.trace_length * 0.002));

    if ((data.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f)) {
        data.penetrate_count = 0;
    }

    if (data.penetrate_count <= 0) {
        return false;
    }

    Vector dummy;
    trace_t trace_exit;
    if (!TraceToExit(dummy, &data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit)) {
        return false;
    }

    surfacedata_t *exit_surface_data = Interfaces::Physics()->GetSurfaceData(trace_exit.surface.surfaceProps);
    int exit_material = exit_surface_data->game.material;

    float exit_surf_penetration_mod = *(float*)((uintptr_t)exit_surface_data + 76);
    float final_damage_modifier = 0.16f;
    float combined_penetration_modifier = 0.0f;

    if(((data.enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71)) {
        combined_penetration_modifier = 3.0f;
        final_damage_modifier = 0.05f;
    } else {
        combined_penetration_modifier = ( enter_surf_penetration_mod + exit_surf_penetration_mod ) * 0.5f;
    }

    if (enter_material == exit_material) {
        if (exit_material == 87 || exit_material == 85) {
            combined_penetration_modifier = 3.0f;
        } else if (exit_material == 76) {
            combined_penetration_modifier = 2.0f;
        }
    }

    float v34 = fmaxf( 0.f, 1.0f / combined_penetration_modifier );
    float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / weaponInfo->GetPenetration()) * 1.25f);
    float thickness = (trace_exit.endpos - data.enter_trace.endpos).Length();

    thickness *= thickness;
    thickness *= v34;
    thickness /= 24.0f;

    float lost_damage = fmaxf( 0.0f, v35 + thickness );

    if (lost_damage > data.current_damage) {
        return false;
    }

    if (lost_damage >= 0.0f) {
        data.current_damage -= lost_damage;
    }

    if (data.current_damage < 1.0f) {
        return false;
    }

    data.src = trace_exit.endpos;
    data.penetrate_count--;

    return true;
}

bool CAutoWalls::SimulateFireBullet(C_CSPlayer* pLocal, C_BaseCombatWeapon* pWeapon, FireBulletData & data) {
    if (!pLocal) {
        return false;
    }

    data.penetrate_count = 4;
    data.trace_length = 0.0f;
    CCSWeaponInfo* weaponData = pWeapon->GetCSWpnData();
    if (weaponData == NULL) {
        return false;
    }

    data.current_damage = (float)weaponData->GetDamage();

    while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f)) {
        data.trace_length_remaining = weaponData->GetRange() - data.trace_length;

        Vector End = data.src + data.direction * data.trace_length_remaining;
        Vector RealEnd = End + (data.direction * 40.f);

        TraceLine(data.src, End, MASK_SHOT, pLocal, &data.enter_trace);
        TraceLine(data.src, RealEnd, MASK_SHOT, pLocal, &data.enter_trace);

        if (data.enter_trace.fraction == 1.0f) {
            break;
        }

        C_CSPlayer* PlayerEntity = (C_CSPlayer*)data.enter_trace.m_pEnt;
        if ((data.enter_trace.hitgroup <= 7) && (data.enter_trace.hitgroup > 0) && (PlayerEntity->GetTeamNum() != pLocal->GetTeamNum())) {
            data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
            data.current_damage *= pow(weaponData->GetRangeModifier(), data.trace_length * 0.002);
            ScaleDamage(data.enter_trace.hitgroup, PlayerEntity, weaponData->GetWeaponArmorRatio(), data.current_damage);

            return true;
        }

        if (!HandleBulletPenetration(weaponData, data)) {
            break;
        }
    }

    return false;
}

float CAutoWalls::GetDamage(C_CSPlayer* pLocal, C_CSPlayer* pEntity, int HitboxIndex) {
    float damage = 0.f;
    Vector dst = pEntity->GetPredictedPosition(HitboxIndex);

    FireBulletData data;
    data.src = pLocal->GetEyePos();
    data.filter.pSkip = pLocal;

    QAngle angles = CMath::CalcAngle(data.src, dst);
    CMath::AngleVectors(angles, data.direction);

    data.direction.NormalizeInPlace();

    C_BaseCombatWeapon* activeWeapon = pLocal->GetActiveWeapon();
    if (!activeWeapon) {
        return -1.0f;
    }

    if (SimulateFireBullet(pLocal, activeWeapon, data)) {
        damage = data.current_damage;
    }

    return damage;
}

float CAutoWalls::GetDamage(C_CSPlayer* pLocal, Vector HitboxPosition) {
    float damage = 0.f;
    Vector dst = HitboxPosition;

    FireBulletData data;
    data.src = pLocal->GetEyePos();
    data.filter.pSkip = pLocal;

    QAngle angles = CMath::CalcAngle(data.src, dst);
    CMath::AngleVectors(angles, data.direction);

    data.direction.NormalizeInPlace();

    C_BaseCombatWeapon* activeWeapon = pLocal->GetActiveWeapon();
    if (!activeWeapon) {
        return -1.0f;
    }

    if (SimulateFireBullet(pLocal, activeWeapon, data)) {
        damage = data.current_damage;
    }

    return damage;
}

