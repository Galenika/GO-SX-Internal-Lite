//
//  menu.hpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 02.03.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#ifndef menu_h
#define menu_h

#include "Engine/DrawManager/draw.h"
#include "Engine/SettingsManager/SimpleIni.h"
#include "Engine/functions.h"

enum FieldType_t {
    FIELDTYPE_BOOL = 0,
    FIELDTYPE_INPUT,
    FIELDTYPE_FLOAT,
    FIELDTYPE_INT,
    FIELDTYPE_COLOR,
    FIELDTYPE_SELECT,
    FIELDTYPE_KEY
};

typedef struct
{
    char    m_szDefinition[12];
    char    m_szShiftDefinition[12];
    char    m_szAltDefinition[12];
}KeyStroke_t;

const std::map<int, const char*> KeyNameForValue = {
    {0, "Not bound ..."},
    {(int)ButtonCode_t::KEY_0, "KEY_0"},
    {(int)ButtonCode_t::KEY_1, "KEY_1"},
    {(int)ButtonCode_t::KEY_2, "KEY_2"},
    {(int)ButtonCode_t::KEY_3, "KEY_3"},
    {(int)ButtonCode_t::KEY_4, "KEY_4"},
    {(int)ButtonCode_t::KEY_5, "KEY_5"},
    {(int)ButtonCode_t::KEY_6, "KEY_6"},
    {(int)ButtonCode_t::KEY_7, "KEY_7"},
    {(int)ButtonCode_t::KEY_8, "KEY_8"},
    {(int)ButtonCode_t::KEY_9, "KEY_9"},
    {(int)ButtonCode_t::KEY_A, "KEY_A"},
    {(int)ButtonCode_t::KEY_B, "KEY_B"},
    {(int)ButtonCode_t::KEY_C, "KEY_C"},
    {(int)ButtonCode_t::KEY_D, "KEY_D"},
    {(int)ButtonCode_t::KEY_E, "KEY_E"},
    {(int)ButtonCode_t::KEY_F, "KEY_F"},
    {(int)ButtonCode_t::KEY_G, "KEY_G"},
    {(int)ButtonCode_t::KEY_H, "KEY_H"},
    {(int)ButtonCode_t::KEY_I, "KEY_I"},
    {(int)ButtonCode_t::KEY_J, "KEY_J"},
    {(int)ButtonCode_t::KEY_K, "KEY_K"},
    {(int)ButtonCode_t::KEY_L, "KEY_L"},
    {(int)ButtonCode_t::KEY_M, "KEY_M"},
    {(int)ButtonCode_t::KEY_N, "KEY_N"},
    {(int)ButtonCode_t::KEY_O, "KEY_O"},
    {(int)ButtonCode_t::KEY_P, "KEY_P"},
    {(int)ButtonCode_t::KEY_Q, "KEY_Q"},
    {(int)ButtonCode_t::KEY_R, "KEY_R"},
    {(int)ButtonCode_t::KEY_S, "KEY_S"},
    {(int)ButtonCode_t::KEY_T, "KEY_T"},
    {(int)ButtonCode_t::KEY_U, "KEY_U"},
    {(int)ButtonCode_t::KEY_V, "KEY_V"},
    {(int)ButtonCode_t::KEY_W, "KEY_W"},
    {(int)ButtonCode_t::KEY_X, "KEY_X"},
    {(int)ButtonCode_t::KEY_Y, "KEY_Y"},
    {(int)ButtonCode_t::KEY_Z, "KEY_Z"},
    {(int)ButtonCode_t::KEY_PAD_0, "KEY_PAD_0"},
    {(int)ButtonCode_t::KEY_PAD_1, "KEY_PAD_1"},
    {(int)ButtonCode_t::KEY_PAD_2, "KEY_PAD_2"},
    {(int)ButtonCode_t::KEY_PAD_3, "KEY_PAD_3"},
    {(int)ButtonCode_t::KEY_PAD_4, "KEY_PAD_4"},
    {(int)ButtonCode_t::KEY_PAD_5, "KEY_PAD_5"},
    {(int)ButtonCode_t::KEY_PAD_6, "KEY_PAD_6"},
    {(int)ButtonCode_t::KEY_PAD_7, "KEY_PAD_7"},
    {(int)ButtonCode_t::KEY_PAD_8, "KEY_PAD_8"},
    {(int)ButtonCode_t::KEY_PAD_9, "KEY_PAD_9"},
    {(int)ButtonCode_t::KEY_PAD_DIVIDE, "KEY_PAD_DIVIDE"},
    {(int)ButtonCode_t::KEY_PAD_MULTIPLY, "KEY_PAD_MULTIPLY"},
    {(int)ButtonCode_t::KEY_PAD_MINUS, "KEY_PAD_MINUS"},
    {(int)ButtonCode_t::KEY_PAD_PLUS, "KEY_PAD_PLUS"},
    {(int)ButtonCode_t::KEY_PAD_ENTER, "KEY_PAD_ENTER"},
    {(int)ButtonCode_t::KEY_PAD_DECIMAL, "KEY_PAD_DECIMAL"},
    {(int)ButtonCode_t::KEY_LBRACKET, "KEY_LBRACKET"},
    {(int)ButtonCode_t::KEY_RBRACKET, "KEY_RBRACKET"},
    {(int)ButtonCode_t::KEY_SEMICOLON, "KEY_SEMICOLON"},
    {(int)ButtonCode_t::KEY_APOSTROPHE, "KEY_APOSTROPHE"},
    {(int)ButtonCode_t::KEY_BACKQUOTE, "KEY_BACKQUOTE"},
    {(int)ButtonCode_t::KEY_COMMA, "KEY_COMMA"},
    {(int)ButtonCode_t::KEY_PERIOD, "KEY_PERIOD"},
    {(int)ButtonCode_t::KEY_SLASH, "KEY_SLASH"},
    {(int)ButtonCode_t::KEY_BACKSLASH, "KEY_BACKSLASH"},
    {(int)ButtonCode_t::KEY_MINUS, "KEY_MINUS"},
    {(int)ButtonCode_t::KEY_EQUAL, "KEY_EQUAL"},
    {(int)ButtonCode_t::KEY_ENTER, "KEY_ENTER"},
    {(int)ButtonCode_t::KEY_SPACE, "KEY_SPACE"},
    {(int)ButtonCode_t::KEY_BACKSPACE, "KEY_BACKSPACE"},
    {(int)ButtonCode_t::KEY_TAB, "KEY_TAB"},
    {(int)ButtonCode_t::KEY_CAPSLOCK, "KEY_CAPSLOCK"},
    {(int)ButtonCode_t::KEY_NUMLOCK, "KEY_NUMLOCK"},
    {(int)ButtonCode_t::KEY_ESCAPE, "KEY_ESCAPE"},
    {(int)ButtonCode_t::KEY_SCROLLLOCK, "KEY_SCROLLLOCK"},
    {(int)ButtonCode_t::KEY_INSERT, "KEY_INSERT"},
    {(int)ButtonCode_t::KEY_DELETE, "KEY_DELETE"},
    {(int)ButtonCode_t::KEY_HOME, "KEY_HOME"},
    {(int)ButtonCode_t::KEY_END, "KEY_END"},
    {(int)ButtonCode_t::KEY_PAGEUP, "KEY_PAGEUP"},
    {(int)ButtonCode_t::KEY_PAGEDOWN, "KEY_PAGEDOWN"},
    {(int)ButtonCode_t::KEY_BREAK, "KEY_BREAK"},
    {(int)ButtonCode_t::KEY_LSHIFT, "KEY_LSHIFT"},
    {(int)ButtonCode_t::KEY_RSHIFT, "KEY_RSHIFT"},
    {(int)ButtonCode_t::KEY_LALT, "KEY_LALT"},
    {(int)ButtonCode_t::KEY_RALT, "KEY_RALT"},
    {(int)ButtonCode_t::KEY_LCONTROL, "KEY_LCONTROL"},
    {(int)ButtonCode_t::KEY_RCONTROL, "KEY_RCONTROL"},
    {(int)ButtonCode_t::KEY_LWIN, "KEY_LWIN"},
    {(int)ButtonCode_t::KEY_RWIN, "KEY_RWIN"},
    {(int)ButtonCode_t::KEY_APP, "KEY_APP"},
    {(int)ButtonCode_t::KEY_UP, "KEY_UP"},
    {(int)ButtonCode_t::KEY_LEFT, "KEY_LEFT"},
    {(int)ButtonCode_t::KEY_DOWN, "KEY_DOWN"},
    {(int)ButtonCode_t::KEY_RIGHT, "KEY_RIGHT"},
    {(int)ButtonCode_t::KEY_F1, "KEY_F1"},
    {(int)ButtonCode_t::KEY_F2, "KEY_F2"},
    {(int)ButtonCode_t::KEY_F3, "KEY_F3"},
    {(int)ButtonCode_t::KEY_F4, "KEY_F4"},
    {(int)ButtonCode_t::KEY_F5, "KEY_F5"},
    {(int)ButtonCode_t::KEY_F6, "KEY_F6"},
    {(int)ButtonCode_t::KEY_F7, "KEY_F7"},
    {(int)ButtonCode_t::KEY_F8, "KEY_F8"},
    {(int)ButtonCode_t::KEY_F9, "KEY_F9"},
    {(int)ButtonCode_t::KEY_F10, "KEY_F10"},
    {(int)ButtonCode_t::KEY_F11, "KEY_F11"},
    {(int)ButtonCode_t::KEY_F12, "KEY_F12"},
    {(int)ButtonCode_t::KEY_CAPSLOCKTOGGLE, "KEY_CAPSLOCKTOGGLE"},
    {(int)ButtonCode_t::KEY_NUMLOCKTOGGLE, "KEY_NUMLOCKTOGGLE"},
    {(int)ButtonCode_t::KEY_SCROLLLOCKTOGGLE, "KEY_SCROLLLOCKTOGGLE"},
    {(int)ButtonCode_t::MOUSE_LEFT, "MOUSE_LEFT"},
    {(int)ButtonCode_t::MOUSE_RIGHT, "MOUSE_RIGHT"},
    {(int)ButtonCode_t::MOUSE_MIDDLE, "MOUSE_MIDDLE"},
    {(int)ButtonCode_t::MOUSE_4, "MOUSE_4"},
    {(int)ButtonCode_t::MOUSE_5, "MOUSE_5"},
    {(int)ButtonCode_t::MOUSE_WHEEL_UP, "MOUSE_WHEEL_UP"},
    {(int)ButtonCode_t::MOUSE_WHEEL_DOWN, "MOUSE_WHEEL_DOWN"}
};

class CHackMenu {
public:
    CHackMenu();
    static CHackMenu* Instance() {
        if(!instance) {
            instance = new CHackMenu();
        }

        return instance;
    }
    void OpenMenu(bool init = false);
    void DrawMenu();
    void CloseMenu();
    bool IsMenuOpen();
    int GetCursorPosition();
    void SetCursorPosition(int position);
    int GetItemCount();
    void HandleInput(int keynum);
    void HandleKey(int keynum);
    void ClickHandler();
    void SetMouseState(bool released = false);
    void SetMousePressed(bool value = false);
    bool IsWaitingForInput();
private:
    void AddTab(const char* section);
    void AddMenuItem(const char *section, const char* key, int col, int row, FieldType_t type);
    void DrawCheckbox(int col, int row, bool value);
    void DrawMouseCursor(int mx, int my);
    void DrawHeader();
    void DrawSaveButton();
    void DrawColorSet(const char *section, const char* key, int x, int y, Color value);
    void DrawSlider(const char *section, const char* key, int x, int y, float value, float minValue, float maxValue);
    void DrawPercentSlider(const char *section, const char* key, int x, int y, int value, int minValue, int maxValue);
    void DrawColorSlider(const char *section, const char* key, int x, int y, int value, Color colorValue);
    void DrawInputField(const char *section, const char *key, int x, int y, std::string value);
    void DrawSelectField(const char *section, const char *key, int x, int y, int value, std::map<int, const char*> list, std::map<int, const char*> values);
    void DrawSelectField(const char *section, const char *key, int x, int y, std::map<int, const char*> list, std::map<int, const char*> values, CSettingsManager* config);
    void SaveMousePosition();
    bool IsMouseOverThis(int x, int y, int w, int h, bool isSelect = false);
    void DrawActiveSelect();
    void DrawKeyField(const char *section, const char *key, int x, int y, int keyValue);
protected:
    static CHackMenu* instance;
    CDrawings* dMgr = nullptr;
    bool MenuOpen = false;
    bool ClickedOnce = false;
    int CursorPosition = 1;
    int baseHeight = 0;
    int baseWidth = 0;
    int baseX = 0;
    int baseY = 0;
    int ItemIndex = 1;
    int tabCount = 0;
    int activeTab = 1;
    int InputIndex = 1;
    int activeInput = 0;
    int selectIndex = 1;
    int activeSelect = 0;
    HFONT headerFont = NULL;
    HFONT sliderFont = NULL;
    HFONT colorSliderFont = NULL;
    bool mouseReleased = false;
    bool mousePressed = false;
    std::string m_szCurrentString[100];
    std::size_t m_nCurrentPos[100];
    bool stringIsEmpty = false;
    int lastMousePosX = 0;
    int lastMousePosY = 0;
    IConVar* mouseEnable = nullptr;
    std::map<const char*, int> tabIndex;
    std::map<int, const char*> indexTab;
    std::map<const char*, int> menuIndex;
    std::map<int, const char*> indexMenu;

    bool IsSelectOpen = false;
    int activeSelectBaseX;
    int activeSelectBaseY;
    const char* activeSelectSection;
    const char* activeSelectKey;
    std::map<int, const char*> activeSelectList;
    std::map<int, const char*> activeSelectValues;
    int lastKey = -5;
    int keyInputIndex = 0;
    int activeKeyInput = -1;
    bool lastKeyChanged = false;
    bool keyInputWaiting = false;
};

#endif /* menu_h */
