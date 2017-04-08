#include "hooks.h"

namespace HookManager
{
    VMT* g_pPanel       = nullptr;
    VMT* g_pClient      = nullptr;
    VMT* g_pModelRender = nullptr;
    VMT* g_pClientMode  = nullptr;
    VMT* g_pGameEvent   = nullptr;

    RecvVarProxyFn g_pSequence = nullptr;

    CDrawings* drawManager = nullptr;

    void Initialize()
    {
        Functions::MessageBox("GO:SX Lite", "Injected!\n", IDOK);
        Interfaces::Cvar()->ConsolePrintf("GO:SX Lite injected!\n");

        while(!Interfaces::Client()) { usleep(500000); }
        g_pClient       = new VMT(Interfaces::Client());
        while(!Interfaces::Panel()) { usleep(500000); }
        g_pPanel        = new VMT(Interfaces::Panel());
        while(!Interfaces::ModelRender()) { usleep(500000); }
        g_pModelRender  = new VMT(Interfaces::ModelRender());
        while(!Interfaces::ClientMode()) { usleep(500000); }
        g_pClientMode   = new VMT(Interfaces::ClientMode());
        while(!Interfaces::GameEventManager()) { usleep(500000); }
        g_pGameEvent    = new VMT(Interfaces::GameEventManager());

        NetvarManager::Instance()->CreateDatabase();

        g_pPanel->HookVM((void*)HPaintTraverse, INDEX_PAINTTRAVERSE);
        g_pPanel->ApplyVMT();

        while(!Interfaces::Engine()->IsInGame()) {
            usleep(200000);
        }

        g_pClient->HookVM((void*)HFrameStageNotify, INDEX_FRAMESTAGENOTIFY);
        g_pClient->HookVM((void*)HINKeyEvent, INDEX_INKEYEVENT);
        g_pClient->ApplyVMT();

        g_pModelRender->HookVM((void*)HDrawModelExecute, INDEX_DRAWMODELEXECUTE);
        g_pModelRender->ApplyVMT();

        g_pClientMode->HookVM((void*)HCreateMove, INDEX_CREATEMOVE);
        g_pClientMode->ApplyVMT();

        g_pGameEvent->HookVM((void*)HFireEventClientSide, INDEX_FIREEVENTCLIENTSIDE);
        g_pGameEvent->ApplyVMT();

        g_pSequence = (RecvVarProxyFn)NetvarManager::Instance()->HookProp("DT_BaseViewModel", "m_nSequence", HSequenceProxyFn);
    }

    int HINKeyEvent(void* thisptr, int eventcode, int keynum, const char* currentbinding) {
        if(INIGET_BOOL("Main", "menu")) {
            if(CHackMenu::Instance()->IsMenuOpen()) {
                if(eventcode == InputEventType_t::IE_ButtonReleased) {
                    if(keynum == ButtonCode_t::KEY_DELETE) {
                        CHackMenu::Instance()->CloseMenu();
                        return g_pClient->GetOriginalMethod<KeyEventFn>(20)(thisptr, eventcode, keynum, currentbinding);
                    }

                    if(keynum == ButtonCode_t::MOUSE_LEFT) {
                        CHackMenu::Instance()->SetMouseState(true);
                    } else {
                        CHackMenu::Instance()->HandleInput(keynum);
                    }
                } else if (eventcode == InputEventType_t::IE_ButtonPressed) {
                    if(keynum == ButtonCode_t::MOUSE_LEFT) {
                        CHackMenu::Instance()->SetMousePressed(true);
                    }
                }
                return 0;

            } else if(!CHackMenu::Instance()->IsMenuOpen()) {
                if(eventcode == InputEventType_t::IE_ButtonReleased) {
                    if(keynum == ButtonCode_t::KEY_DELETE) {
                        CHackMenu::Instance()->OpenMenu();
                    }
                }
            }
        }
        return g_pClient->GetOriginalMethod<KeyEventFn>(20)(thisptr, eventcode, keynum, currentbinding);
    }

    void HFrameStageNotify(void* thisptr, ClientFrameStage_t stage) {
        g_pClient->GetOriginalMethod<FrameStageNotifyFn>(36)(thisptr, stage);

        if(stage == ClientFrameStage_t::FRAME_START) {
            if(!CHackMenu::Instance()->IsMenuOpen()) {
                CSettingsManager::Instance("settings.ini")->ReloadSettings();
                CSettingsManager::Instance("menu.ini")->ReloadSettings();
                CSettingsManager::Instance("weapons.ini")->ReloadSettings();
            }
        }

        if(INIGET_BOOL("Main", "enabled")) {
            FeatureManager::FrameStageNotify(stage);
        }
    }
    
    void HPaintTraverse(void* thisptr, VPANEL vguiPanel, bool forceRepaint, bool allowForce) {
        g_pPanel->GetOriginalMethod<PaintTraverseFn>(42)(thisptr, vguiPanel, forceRepaint, allowForce);
        if(INIGET_BOOL("Main", "enabled")) {
            FeatureManager::PaintTraverse(thisptr, vguiPanel, forceRepaint, allowForce);
        }

        if(INIGET_BOOL("Main", "menu")) {
            std::string szPanelName = Interfaces::Panel()->GetName(vguiPanel);
            if (szPanelName.find("FocusOverlayPanel") != std::string::npos) {
                if(CHackMenu::Instance()->IsMenuOpen()) {
                    CHackMenu::Instance()->DrawMenu();
                }
            }
        }
    }

    void HDrawModelExecute(void* thisptr, IMatRenderContext* context, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld) {
        g_pModelRender->ReleaseVMT();

        FeatureManager::DrawModelExecute(context, state, pInfo, pCustomBoneToWorld);

        g_pModelRender->ApplyVMT();
    }
    
    bool HCreateMove(void* thisptr, float sample_input_frametime, CUserCmd* pCmd) {
        g_pClientMode->GetOriginalMethod<CreateMoveFn>(25)(thisptr, sample_input_frametime, pCmd);
        if(INIGET_BOOL("Main", "enabled")) {
            FeatureManager::CreateMove(thisptr, sample_input_frametime, pCmd);
        }
        
        return false;
    }

    bool HFireEventClientSide(IGameEventManager2* thisptr, IGameEvent* event) {
        if(INIGET_BOOL("Main", "enabled")) {

        }

        return g_pGameEvent->GetOriginalMethod<FireEventClientSideFn>(10)(thisptr, event);
    }

    inline const int RandomSequence(int low, int high) {
        return (rand() % (high - low + 1) + low);
    }

    void HSequenceProxyFn(const CRecvProxyData *pDataConst, void *pStruct, void *pOut) {
        CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);
        C_BaseViewModel* pViewModel = (C_BaseViewModel*)pStruct;
        if(!pViewModel) {
            return g_pSequence(pDataConst, pStruct, pOut);
        }
        C_BaseEntity* pOwner = (C_BaseEntity*)Interfaces::EntityList()->GetClientEntityFromHandle(pViewModel->GetOwner());
        if (pViewModel && pOwner) {
            if (pOwner->EntIndex() == Interfaces::Engine()->GetLocalPlayer()) {
                const model_t* knife_model = Interfaces::ModelInfo()->GetModel(*pViewModel->GetModelIndex());
                const char* model_filename = Interfaces::ModelInfo()->GetModelName(knife_model);
                int m_nSequence = (int)pData->m_Value.m_Int;

                if (!strcmp(model_filename, "models/weapons/v_knife_butterfly.mdl")) {
                    switch (m_nSequence) {
                        case SEQUENCE_DEFAULT_DRAW:
                            m_nSequence = RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2); break;
                        case SEQUENCE_DEFAULT_LOOKAT01:
                            m_nSequence = RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03); break;
                        default:
                            m_nSequence++;
                    }
                } else if (!strcmp(model_filename, "models/weapons/v_knife_falchion_advanced.mdl")) {
                    switch (m_nSequence) {
                        case SEQUENCE_DEFAULT_IDLE2:
                            m_nSequence = SEQUENCE_FALCHION_IDLE1; break;
                        case SEQUENCE_DEFAULT_HEAVY_MISS1:
                            m_nSequence = RandomSequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP); break;
                        case SEQUENCE_DEFAULT_LOOKAT01:
                            m_nSequence = RandomSequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02); break;
                        case SEQUENCE_DEFAULT_DRAW:
                        case SEQUENCE_DEFAULT_IDLE1:
                            break;
                        default:
                            m_nSequence--;
                    }
                } else if (!strcmp(model_filename, "models/weapons/v_knife_push.mdl")) {
                    switch (m_nSequence) {
                        case SEQUENCE_DEFAULT_IDLE2:
                            m_nSequence = SEQUENCE_DAGGERS_IDLE1; break;
                        case SEQUENCE_DEFAULT_LIGHT_MISS1:
                        case SEQUENCE_DEFAULT_LIGHT_MISS2:
                            m_nSequence = RandomSequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5); break;
                        case SEQUENCE_DEFAULT_HEAVY_MISS1:
                            m_nSequence = RandomSequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1); break;
                        case SEQUENCE_DEFAULT_HEAVY_HIT1:
                        case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
                        case SEQUENCE_DEFAULT_LOOKAT01:
                            m_nSequence += 3; break;
                        case SEQUENCE_DEFAULT_DRAW:
                        case SEQUENCE_DEFAULT_IDLE1:
                            break;
                        default:
                            m_nSequence += 2;
                    }
                } else if (!strcmp(model_filename, "models/weapons/v_knife_survival_bowie.mdl")) {
                    switch (m_nSequence) {
                        case SEQUENCE_DEFAULT_DRAW:
                        case SEQUENCE_DEFAULT_IDLE1:
                            break;
                        case SEQUENCE_DEFAULT_IDLE2:
                            m_nSequence = SEQUENCE_BOWIE_IDLE1; break;
                        default:
                            m_nSequence--;
                    }
                }

                pData->m_Value.m_Int = m_nSequence;
            }
        }

        return g_pSequence(pData, pStruct, pOut);
    }
}

