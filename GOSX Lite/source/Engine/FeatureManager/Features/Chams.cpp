#include "Chams.h"

CChams::CChams() {
    hidden_tex = CreateMaterial(false, false, true, true, true, true, true);
    visible_tex = CreateMaterial(true, false, true, true, true, true, true);
    hidden_flat = CreateMaterial(false, true, true, true, true, true, true);
    visible_flat = CreateMaterial(true, true, true, true, true, true, true);
}

std::map<int, const char*> CChams::GetChamList() {
    std::map<int, const char*> select;

    select[0] = "textured"; // HITBOX_HEAD
    select[1] = "flat"; // HITBOX_NECK

    return select;
}

std::map<int, const char*> CChams::GetChamValues() {
    std::map<int, const char*> values;

    values[0] = "Textured";
    values[1] = "Flat";

    return values;
}

void CChams::AddMaterial(std::string filename, std::string type, std::string texture, bool ignorez, bool nofog, bool model, bool nocull, bool halflambert, bool gluelook) {
    if (!Func::IsFileExists(Func::GetWorkingPath().append("csgo/materials/").append(filename).append(".vmt"))) {
        std::stringstream ss;

        ss << "\"" + type + "\"" << std::endl;
        ss << "{" << std::endl;
        ss << "\t\"$basetexture\" \"" + texture + "\"" << std::endl;
        ss << "\t\"$ignorez\" \"" + std::to_string(ignorez) + "\"" << std::endl;
        ss << "\t\"$nofog\" \"" + std::to_string(nofog) + "\"" << std::endl;
        ss << "\t\"$model\" \"" + std::to_string(model) + "\"" << std::endl;
        ss << "\t\"$nocull\" \"" + std::to_string(nocull) + "\"" << std::endl;
        ss << "\t\"$halflambert\" \"" + std::to_string(halflambert) + "\"" << std::endl;
        if (gluelook && !texture.find("UnlitGeneric")) {
            ss << "\t\"$envmap\" \"env_cubemap\"" << std::endl;
            ss << "\t\"$normalmapalphaenvmapmask\" 1" << std::endl;
            ss << "\t\"$envmapcontrast\" 1" << std::endl;
        }
        ss << "}" << std::endl;

        std::ofstream(Func::GetWorkingPath().append("csgo/materials/").append(filename).append(".vmt").c_str()) << ss.str();
    }
}

IMaterial* CChams::CreateMaterial(bool ignorez, bool flat, bool nofog, bool model, bool nocull, bool halflambert, bool gluelook) {
    IMaterial* createdMaterial = nullptr;

    if (gluelook) {
        if (ignorez) {
            if (flat) {
                if (!Func::IsFileExists(Func::GetWorkingPath().append("csgo/materials/").append("chamsmat_flat_ignorez").append(".vmt"))) {
                    AddMaterial("chamsmat_flat_ignorez", "UnlitGeneric", "VGUI/white_additive", ignorez, nofog, model, nocull, halflambert, gluelook);
                }
                createdMaterial = Interfaces::MaterialSystem()->FindMaterial("chamsmat_flat_ignorez");
            } else {
                if (!Func::IsFileExists(Func::GetWorkingPath().append("csgo/materials/").append("chamsmat_ignorez").append(".vmt"))) {
                    AddMaterial("chamsmat_ignorez", "VertexLitGeneric", "VGUI/white_additive", ignorez, nofog, model, nocull, halflambert, gluelook);
                }
                createdMaterial = Interfaces::MaterialSystem()->FindMaterial("chamsmat_ignorez");
            }
        } else {
            if (flat) {
                if (!Func::IsFileExists(Func::GetWorkingPath().append("csgo/materials/").append("chamsmat_flat").append(".vmt"))) {
                    AddMaterial("chamsmat_flat", "UnlitGeneric", "VGUI/white_additive", ignorez, nofog, model, nocull, halflambert, gluelook);
                }
                createdMaterial = Interfaces::MaterialSystem()->FindMaterial("chamsmat_flat");
            } else {
                if (!Func::IsFileExists(Func::GetWorkingPath().append("csgo/materials/").append("chamsmat").append(".vmt"))) {
                    AddMaterial("chamsmat", "VertexLitGeneric", "VGUI/white_additive", ignorez, nofog, model, nocull, halflambert, gluelook);
                }
                createdMaterial = Interfaces::MaterialSystem()->FindMaterial("chamsmat");
            }
        }
    }

    createdMaterial->IncrementReferenceCount();

    return createdMaterial;
}

void CChams::apply(IMatRenderContext* oriContext, const DrawModelState_t &oriState, const ModelRenderInfo_t &oriPInfo, matrix3x4_t *oriPCustomBoneToWorld)
{
    // Check if the Hack is enabled
    if (INIGET_BOOL("Main", "enabled")) {
        // Check if Chams are enabled
        if (INIGET_BOOL("Chams", "enabled")) {
            // Check if the current model is valid
            if (oriPInfo.pModel) {
                // Get the model name
                std::string szModelName = Interfaces::ModelInfo()->GetModelName(oriPInfo.pModel);
                // Check if player chams are active
                if (INIGET_BOOL("Chams", "players")) {
                    // Does the current model contain a player model?
                    if (szModelName.find("models/player/") != std::string::npos) {
                        // Get the entity from that model
                        C_CSPlayer* pModelPlayer = (C_CSPlayer*)Interfaces::EntityList()->GetClientEntity(oriPInfo.entity_index);
                        // Get the LocalPlayer entity
                        C_CSPlayer* LocalPlayer = C_CSPlayer::GetLocalPlayer();

                        // Check if the target and localplayer entity is valid
                        if (pModelPlayer && pModelPlayer->IsValidLivePlayer() && LocalPlayer) {
                            // Make a team check and compare them to the hack settings
                            if (
                               (INIGET_BOOL("Chams", "allies") && pModelPlayer->GetTeamNum() == LocalPlayer->GetTeamNum()) ||
                               (INIGET_BOOL("Chams", "enemies") && pModelPlayer->GetTeamNum() != LocalPlayer->GetTeamNum())
                            ) {
                                // Define cham colors
                                Color ChamsVisibleColor = Color(0, 0, 0, 0);
                                Color ChamsColor = Color(0, 0, 0, 0);
                                // Set the colors for team T
                                if (pModelPlayer->GetTeamNum() == EntityTeam::TEAM_T) {
                                    ChamsColor = INIGET_COLOR("Colors", "color_t");
                                    ChamsVisibleColor = INIGET_COLOR("Colors", "color_t_visible");
                                // Set the colors for team CT
                                } else if (pModelPlayer->GetTeamNum() == EntityTeam::TEAM_CT) {
                                    ChamsColor = INIGET_COLOR("Colors", "color_ct");
                                    ChamsVisibleColor = INIGET_COLOR("Colors", "color_ct_visible");
                                }

                                IMaterial* cham_visible = visible_tex;
                                IMaterial* cham_hidden = hidden_tex;
                                if(INIGET_INT("Chams", "chams_type") == 1) {
                                    cham_visible = visible_flat;
                                    cham_hidden = hidden_flat;
                                }

                                // Is the wallhack active? When yes force the new material to that model
                                if (INIGET_BOOL("Chams", "wallhack")) {
                                    ForceMaterial(cham_visible, ChamsColor, pModelPlayer->GetImmune());
                                    Interfaces::ModelRender()->DrawModelExecute(oriContext, oriState, oriPInfo, oriPCustomBoneToWorld);
                                }
                                // Force the visible material to the model
                                ForceMaterial(cham_hidden, ChamsVisibleColor, pModelPlayer->GetImmune());
                                Interfaces::ModelRender()->DrawModelExecute(oriContext, oriState, oriPInfo, oriPCustomBoneToWorld);
                            }
                        }
                    }
                }
            }
        }
    }

    // call the origin method to apply all chams
    Interfaces::ModelRender()->DrawModelExecute(oriContext, oriState, oriPInfo, oriPCustomBoneToWorld);
    Interfaces::ModelRender()->ForcedMaterialOverride(nullptr);
}

void CChams::ForceMaterial(IMaterial* material, Color color, bool immune = false)
{
    if (material)
    {
        float alpha = 1.f;
        if (immune) {
            alpha = 0.5f;
        }

        material->AlphaModulate(alpha);
        material->ColorModulate(color.r() / 255.0f, color.g() / 255.0f, color.b() / 255.0f);

        Interfaces::ModelRender()->ForcedMaterialOverride(material);
    }
}
