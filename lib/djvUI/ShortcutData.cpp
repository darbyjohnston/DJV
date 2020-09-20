// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/ShortcutData.h>

#include <djvSystem/TextSystem.h>

#include <djvCore/StringFunc.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        ShortcutData::ShortcutData(int key, int modifiers) :
            key(key),
            modifiers(modifiers)
        {}

        int ShortcutData::getSystemModifier()
        {
#if defined(DJV_PLATFORM_MACOS)
            return GLFW_MOD_SUPER;
#else
            return GLFW_MOD_CONTROL;
#endif
        }

        std::map<int, std::string> ShortcutData::getKeyStrings()
        {
            return
            {
                { GLFW_KEY_SPACE, DJV_TEXT("key_space") },
                { GLFW_KEY_APOSTROPHE, DJV_TEXT("key_apostrophe") },
                { GLFW_KEY_COMMA, DJV_TEXT("key_comma") },
                { GLFW_KEY_MINUS, DJV_TEXT("key_minus") },
                { GLFW_KEY_PERIOD, DJV_TEXT("key_period") },
                { GLFW_KEY_SLASH, DJV_TEXT("key_slash") },
                { GLFW_KEY_0, DJV_TEXT("key_0") },
                { GLFW_KEY_1, DJV_TEXT("key_1") },
                { GLFW_KEY_2, DJV_TEXT("key_2") },
                { GLFW_KEY_3, DJV_TEXT("key_3") },
                { GLFW_KEY_4, DJV_TEXT("key_4") },
                { GLFW_KEY_5, DJV_TEXT("key_5") },
                { GLFW_KEY_6, DJV_TEXT("key_6") },
                { GLFW_KEY_7, DJV_TEXT("key_7") },
                { GLFW_KEY_8, DJV_TEXT("key_8") },
                { GLFW_KEY_9, DJV_TEXT("key_9") },
                { GLFW_KEY_SEMICOLON, DJV_TEXT("key_semicolon") },
                { GLFW_KEY_EQUAL, DJV_TEXT("key_equal") },
                { GLFW_KEY_A, DJV_TEXT("key_a") },
                { GLFW_KEY_B, DJV_TEXT("key_b") },
                { GLFW_KEY_C, DJV_TEXT("key_c") },
                { GLFW_KEY_D, DJV_TEXT("key_d") },
                { GLFW_KEY_E, DJV_TEXT("key_e") },
                { GLFW_KEY_F, DJV_TEXT("key_f") },
                { GLFW_KEY_G, DJV_TEXT("key_g") },
                { GLFW_KEY_H, DJV_TEXT("key_h") },
                { GLFW_KEY_I, DJV_TEXT("key_i") },
                { GLFW_KEY_J, DJV_TEXT("key_j") },
                { GLFW_KEY_K, DJV_TEXT("key_k") },
                { GLFW_KEY_L, DJV_TEXT("key_l") },
                { GLFW_KEY_M, DJV_TEXT("key_m") },
                { GLFW_KEY_N, DJV_TEXT("key_n") },
                { GLFW_KEY_O, DJV_TEXT("key_o") },
                { GLFW_KEY_P, DJV_TEXT("key_p") },
                { GLFW_KEY_Q, DJV_TEXT("key_q") },
                { GLFW_KEY_R, DJV_TEXT("key_r") },
                { GLFW_KEY_S, DJV_TEXT("key_s") },
                { GLFW_KEY_T, DJV_TEXT("key_t") },
                { GLFW_KEY_U, DJV_TEXT("key_u") },
                { GLFW_KEY_V, DJV_TEXT("key_v") },
                { GLFW_KEY_W, DJV_TEXT("key_w") },
                { GLFW_KEY_X, DJV_TEXT("key_x") },
                { GLFW_KEY_Y, DJV_TEXT("key_y") },
                { GLFW_KEY_Z, DJV_TEXT("key_z") },
                { GLFW_KEY_LEFT_BRACKET, DJV_TEXT("key_leftbracket") },
                { GLFW_KEY_BACKSLASH, DJV_TEXT("key_backslash") },
                { GLFW_KEY_RIGHT_BRACKET, DJV_TEXT("key_rightbracket") },
                { GLFW_KEY_GRAVE_ACCENT, DJV_TEXT("key_graveaccent") },
                { GLFW_KEY_WORLD_1, DJV_TEXT("key_world1") },
                { GLFW_KEY_WORLD_2, DJV_TEXT("key_world2") },
                { GLFW_KEY_ESCAPE, DJV_TEXT("key_escape") },
                { GLFW_KEY_ENTER, DJV_TEXT("key_enter") },
                { GLFW_KEY_TAB, DJV_TEXT("key_tab") },
                { GLFW_KEY_BACKSPACE, DJV_TEXT("key_backspace") },
                { GLFW_KEY_INSERT, DJV_TEXT("key_insert") },
                { GLFW_KEY_DELETE, DJV_TEXT("key_delete") },
                { GLFW_KEY_RIGHT, DJV_TEXT("key_rightarrow") },
                { GLFW_KEY_LEFT, DJV_TEXT("key_leftarrow") },
                { GLFW_KEY_DOWN, DJV_TEXT("key_downarrow") },
                { GLFW_KEY_UP, DJV_TEXT("key_uparrow") },
                { GLFW_KEY_PAGE_UP, DJV_TEXT("key_pageup") },
                { GLFW_KEY_PAGE_DOWN, DJV_TEXT("key_pagedown") },
                { GLFW_KEY_HOME, DJV_TEXT("key_home") },
                { GLFW_KEY_END, DJV_TEXT("key_end") },
                { GLFW_KEY_CAPS_LOCK, DJV_TEXT("key_capslock") },
                { GLFW_KEY_SCROLL_LOCK, DJV_TEXT("key_scrolllock") },
                { GLFW_KEY_NUM_LOCK, DJV_TEXT("key_numlock") },
                { GLFW_KEY_PRINT_SCREEN, DJV_TEXT("key_printscreen") },
                { GLFW_KEY_F1, DJV_TEXT("key_f1") },
                { GLFW_KEY_F2, DJV_TEXT("key_f2") },
                { GLFW_KEY_F3, DJV_TEXT("key_f3") },
                { GLFW_KEY_F4, DJV_TEXT("key_f4") },
                { GLFW_KEY_F5, DJV_TEXT("key_f5") },
                { GLFW_KEY_F6, DJV_TEXT("key_f6") },
                { GLFW_KEY_F7, DJV_TEXT("key_f7") },
                { GLFW_KEY_F8, DJV_TEXT("key_f8") },
                { GLFW_KEY_F9, DJV_TEXT("key_f9") },
                { GLFW_KEY_F10, DJV_TEXT("key_f10") },
                { GLFW_KEY_F11, DJV_TEXT("key_f11") },
                { GLFW_KEY_F12, DJV_TEXT("key_f12") },
                { GLFW_KEY_F13, DJV_TEXT("key_f13") },
                { GLFW_KEY_F14, DJV_TEXT("key_f14") },
                { GLFW_KEY_F15, DJV_TEXT("key_f15") },
                { GLFW_KEY_F16, DJV_TEXT("key_f16") },
                { GLFW_KEY_F17, DJV_TEXT("key_f17") },
                { GLFW_KEY_F18, DJV_TEXT("key_f18") },
                { GLFW_KEY_F19, DJV_TEXT("key_f19") },
                { GLFW_KEY_F20, DJV_TEXT("key_f20") },
                { GLFW_KEY_F21, DJV_TEXT("key_f21") },
                { GLFW_KEY_F22, DJV_TEXT("key_f22") },
                { GLFW_KEY_F23, DJV_TEXT("key_f23") },
                { GLFW_KEY_F24, DJV_TEXT("key_f24") },
                { GLFW_KEY_F25, DJV_TEXT("key_f25") },
                { GLFW_KEY_KP_0, DJV_TEXT("key_keypad0") },
                { GLFW_KEY_KP_1, DJV_TEXT("key_keypad1") },
                { GLFW_KEY_KP_2, DJV_TEXT("key_keypad2") },
                { GLFW_KEY_KP_3, DJV_TEXT("key_keypad3") },
                { GLFW_KEY_KP_4, DJV_TEXT("key_keypad4") },
                { GLFW_KEY_KP_5, DJV_TEXT("key_keypad5") },
                { GLFW_KEY_KP_6, DJV_TEXT("key_keypad6") },
                { GLFW_KEY_KP_7, DJV_TEXT("key_keypad7") },
                { GLFW_KEY_KP_8, DJV_TEXT("key_keypad8") },
                { GLFW_KEY_KP_9, DJV_TEXT("key_keypad9") },
                { GLFW_KEY_KP_DECIMAL, DJV_TEXT("key_keypaddecimal") },
                { GLFW_KEY_KP_DIVIDE, DJV_TEXT("key_keypaddivide") },
                { GLFW_KEY_KP_MULTIPLY, DJV_TEXT("key_keypadmultiply") },
                { GLFW_KEY_KP_SUBTRACT, DJV_TEXT("key_keypadsubstract") },
                { GLFW_KEY_KP_ADD, DJV_TEXT("key_keypadadd") },
                { GLFW_KEY_KP_ENTER, DJV_TEXT("key_keypadenter") },
                { GLFW_KEY_KP_EQUAL, DJV_TEXT("key_keypadequal") },
                { GLFW_KEY_LEFT_SHIFT, DJV_TEXT("key_leftshift") },
                { GLFW_KEY_LEFT_CONTROL, DJV_TEXT("key_leftcontrol") },
                { GLFW_KEY_LEFT_ALT, DJV_TEXT("key_leftalt") },
                { GLFW_KEY_LEFT_SUPER, DJV_TEXT("key_leftcommand") },
                { GLFW_KEY_RIGHT_SHIFT, DJV_TEXT("key_rightshift") },
                { GLFW_KEY_RIGHT_CONTROL, DJV_TEXT("key_rightcontrol") },
                { GLFW_KEY_RIGHT_ALT, DJV_TEXT("key_rightalt") },
                { GLFW_KEY_RIGHT_SUPER, DJV_TEXT("key_rightcommand") },
                { GLFW_KEY_MENU, DJV_TEXT("key_menu") },
            };
        }

        std::map<int, std::string> ShortcutData::getModifierStrings()
        {
            return
            {
                { GLFW_MOD_SHIFT, DJV_TEXT("key_shift") },
                { GLFW_MOD_CONTROL, DJV_TEXT("key_ctrl") },
                { GLFW_MOD_ALT, DJV_TEXT("key_alt") },
                { GLFW_MOD_SUPER, DJV_TEXT("key_command") }
            };
        }

        std::string ShortcutData::keyToString(int key)
        {
            const auto& data = getKeyStrings();
            const auto i = data.find(key);
            return i != data.end() ? i->second : std::string();
        }

        std::string ShortcutData::modifierToString(int key)
        {
            const auto& data = getModifierStrings();
            const auto i = data.find(key);
            return i != data.end() ? i->second : std::string();
        }

        int ShortcutData::keyFromString(const std::string& value)
        {
            int out = 0;
            for (const auto& i : getKeyStrings())
            {
                if (value == i.second)
                {
                    out = i.first;
                    break;
                }
            }
            return out;
        }

        int ShortcutData::modifierFromString(const std::string& value)
        {
            int out = 0;
            for (const auto& i : getModifierStrings())
            {
                if (value == i.second)
                {
                    out = i.first;
                    break;
                }
            }
            return out;
        }

        std::string ShortcutData::getText(const ShortcutData& shortcutData, const std::shared_ptr<System::TextSystem>& textSystem)
        {
            return getText(shortcutData.key, shortcutData.modifiers, textSystem);
        }

        std::string ShortcutData::getText(int key, int keyModifiers, const std::shared_ptr<System::TextSystem>& textSystem)
        {
            std::vector<std::string> out;
            if (keyModifiers & GLFW_MOD_SHIFT)
            {
                out.push_back(textSystem->getText(modifierToString(GLFW_MOD_SHIFT)));
            }
            if (keyModifiers & GLFW_MOD_CONTROL)
            {
                out.push_back(textSystem->getText(modifierToString(GLFW_MOD_CONTROL)));
            }
            if (keyModifiers & GLFW_MOD_ALT)
            {
                out.push_back(textSystem->getText(modifierToString(GLFW_MOD_ALT)));
            }
            if (keyModifiers & GLFW_MOD_SUPER)
            {
                out.push_back(textSystem->getText(modifierToString(GLFW_MOD_SUPER)));
            }
            out.push_back(textSystem->getText(keyToString(key)));
            return String::join(out, " ");
        }

        ShortcutDataPair::ShortcutDataPair()
        {}

        ShortcutDataPair::ShortcutDataPair(const ShortcutData& primary) :
            primary(primary)
        {}

        ShortcutDataPair::ShortcutDataPair(const ShortcutData& primary, const ShortcutData& secondary) :
            primary(primary),
            secondary(secondary)
        {}

    } // namespace UI
} // namespace djv
