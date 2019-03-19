//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvUI/Shortcut.h>

#include <djvCore/String.h>
#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

#include <map>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Shortcut::Private
        {
            std::shared_ptr<ValueSubject<int> > shortcutKey;
            std::shared_ptr<ValueSubject<int> > shortcutModifiers;
            std::function<void(void)> callback;
        };

        void Shortcut::_init()
        {
            _p->shortcutKey = ValueSubject<int>::create(0);
            _p->shortcutModifiers = ValueSubject<int>::create(0);
        }

        Shortcut::Shortcut() :
            _p(new Private)
        {}

        Shortcut::~Shortcut()
        {}

        std::shared_ptr<Shortcut> Shortcut::create()
        {
            auto out = std::shared_ptr<Shortcut>(new Shortcut);
            out->_init();
            return out;
        }

        std::shared_ptr<Shortcut> Shortcut::create(int key)
        {
            auto out = std::shared_ptr<Shortcut>(new Shortcut);
            out->_init();
            out->setShortcutKey(key);
            return out;
        }

        std::shared_ptr<Shortcut> Shortcut::create(int key, int modifiers)
        {
            auto out = std::shared_ptr<Shortcut>(new Shortcut);
            out->_init();
            out->setShortcutKey(key);
            out->setShortcutModifiers(modifiers);
            return out;
        }

        std::shared_ptr<IValueSubject<int> > Shortcut::observeShortcutKey() const
        {
            return _p->shortcutKey;
        }

        void Shortcut::setShortcutKey(int value)
        {
            _p->shortcutKey->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<int> > Shortcut::observeShortcutModifiers() const
        {
            return _p->shortcutModifiers;
        }

        void Shortcut::setShortcutModifiers(int value)
        {
            _p->shortcutModifiers->setIfChanged(value);
        }

        void Shortcut::setCallback(const std::function<void(void)> & value)
        {
            _p->callback = value;
        }

        void Shortcut::doCallback()
        {
            if (_p->callback)
            {
                _p->callback();
            }
        }

        const std::string & Shortcut::getKeyString(int key)
        {
            static const std::map<int, std::string> data =
            {
                { GLFW_KEY_SPACE, DJV_TEXT("Space") },
                { GLFW_KEY_APOSTROPHE, DJV_TEXT("Apostrophe") },
                { GLFW_KEY_COMMA, DJV_TEXT("Comma") },
                { GLFW_KEY_MINUS, DJV_TEXT("Minus") },
                { GLFW_KEY_PERIOD, DJV_TEXT("Period") },
                { GLFW_KEY_SLASH, DJV_TEXT("Slash") },
                { GLFW_KEY_0, DJV_TEXT("0") },
                { GLFW_KEY_1, DJV_TEXT("1") },
                { GLFW_KEY_2, DJV_TEXT("2") },
                { GLFW_KEY_3, DJV_TEXT("3") },
                { GLFW_KEY_4, DJV_TEXT("4") },
                { GLFW_KEY_5, DJV_TEXT("5") },
                { GLFW_KEY_6, DJV_TEXT("6") },
                { GLFW_KEY_7, DJV_TEXT("7") },
                { GLFW_KEY_8, DJV_TEXT("8") },
                { GLFW_KEY_9, DJV_TEXT("9") },
                { GLFW_KEY_SEMICOLON, DJV_TEXT("Semicolon") },
                { GLFW_KEY_EQUAL, DJV_TEXT("Equal") },
                { GLFW_KEY_A, DJV_TEXT("A") },
                { GLFW_KEY_B, DJV_TEXT("B") },
                { GLFW_KEY_C, DJV_TEXT("C") },
                { GLFW_KEY_D, DJV_TEXT("D") },
                { GLFW_KEY_E, DJV_TEXT("E") },
                { GLFW_KEY_F, DJV_TEXT("F") },
                { GLFW_KEY_G, DJV_TEXT("G") },
                { GLFW_KEY_H, DJV_TEXT("H") },
                { GLFW_KEY_I, DJV_TEXT("I") },
                { GLFW_KEY_J, DJV_TEXT("J") },
                { GLFW_KEY_K, DJV_TEXT("K") },
                { GLFW_KEY_L, DJV_TEXT("L") },
                { GLFW_KEY_M, DJV_TEXT("M") },
                { GLFW_KEY_N, DJV_TEXT("N") },
                { GLFW_KEY_O, DJV_TEXT("O") },
                { GLFW_KEY_P, DJV_TEXT("P") },
                { GLFW_KEY_Q, DJV_TEXT("Q") },
                { GLFW_KEY_R, DJV_TEXT("R") },
                { GLFW_KEY_S, DJV_TEXT("S") },
                { GLFW_KEY_T, DJV_TEXT("T") },
                { GLFW_KEY_U, DJV_TEXT("U") },
                { GLFW_KEY_V, DJV_TEXT("V") },
                { GLFW_KEY_W, DJV_TEXT("W") },
                { GLFW_KEY_X, DJV_TEXT("X") },
                { GLFW_KEY_Y, DJV_TEXT("Y") },
                { GLFW_KEY_Z, DJV_TEXT("Z") },
                { GLFW_KEY_LEFT_BRACKET, DJV_TEXT("LeftBracket") },
                { GLFW_KEY_BACKSLASH, DJV_TEXT("Backslash") },
                { GLFW_KEY_RIGHT_BRACKET, DJV_TEXT("RightBracket") },
                { GLFW_KEY_GRAVE_ACCENT, DJV_TEXT("GraveAccent") },
                { GLFW_KEY_WORLD_1, DJV_TEXT("World1") },
                { GLFW_KEY_WORLD_2, DJV_TEXT("World2") },
                { GLFW_KEY_ESCAPE, DJV_TEXT("Escape") },
                { GLFW_KEY_ENTER, DJV_TEXT("Enter") },
                { GLFW_KEY_TAB, DJV_TEXT("Tab") },
                { GLFW_KEY_BACKSPACE, DJV_TEXT("Backspace") },
                { GLFW_KEY_INSERT, DJV_TEXT("Insert") },
                { GLFW_KEY_DELETE, DJV_TEXT("Delete") },
                { GLFW_KEY_RIGHT, DJV_TEXT("RightArrow") },
                { GLFW_KEY_LEFT, DJV_TEXT("LeftArrow") },
                { GLFW_KEY_DOWN, DJV_TEXT("DownArrow") },
                { GLFW_KEY_UP, DJV_TEXT("UpArrow") },
                { GLFW_KEY_PAGE_UP, DJV_TEXT("PageUp") },
                { GLFW_KEY_PAGE_DOWN, DJV_TEXT("PageDown") },
                { GLFW_KEY_HOME, DJV_TEXT("Home") },
                { GLFW_KEY_END, DJV_TEXT("End") },
                { GLFW_KEY_CAPS_LOCK, DJV_TEXT("CapsLock") },
                { GLFW_KEY_SCROLL_LOCK, DJV_TEXT("ScrollLock") },
                { GLFW_KEY_NUM_LOCK, DJV_TEXT("NumLock") },
                { GLFW_KEY_PRINT_SCREEN, DJV_TEXT("PrintScreen") },
                { GLFW_KEY_F1, DJV_TEXT("F1") },
                { GLFW_KEY_F2, DJV_TEXT("F2") },
                { GLFW_KEY_F3, DJV_TEXT("F3") },
                { GLFW_KEY_F4, DJV_TEXT("F4") },
                { GLFW_KEY_F5, DJV_TEXT("F5") },
                { GLFW_KEY_F6, DJV_TEXT("F6") },
                { GLFW_KEY_F7, DJV_TEXT("F7") },
                { GLFW_KEY_F8, DJV_TEXT("F8") },
                { GLFW_KEY_F9, DJV_TEXT("F9") },
                { GLFW_KEY_F10, DJV_TEXT("F10") },
                { GLFW_KEY_F11, DJV_TEXT("F11") },
                { GLFW_KEY_F12, DJV_TEXT("F12") },
                { GLFW_KEY_F13, DJV_TEXT("F13") },
                { GLFW_KEY_F14, DJV_TEXT("F14") },
                { GLFW_KEY_F15, DJV_TEXT("F15") },
                { GLFW_KEY_F16, DJV_TEXT("F16") },
                { GLFW_KEY_F17, DJV_TEXT("F17") },
                { GLFW_KEY_F18, DJV_TEXT("F18") },
                { GLFW_KEY_F19, DJV_TEXT("F19") },
                { GLFW_KEY_F20, DJV_TEXT("F20") },
                { GLFW_KEY_F21, DJV_TEXT("F21") },
                { GLFW_KEY_F22, DJV_TEXT("F22") },
                { GLFW_KEY_F23, DJV_TEXT("F23") },
                { GLFW_KEY_F24, DJV_TEXT("F24") },
                { GLFW_KEY_F25, DJV_TEXT("F25") },
                { GLFW_KEY_KP_0, DJV_TEXT("KeyPad0") },
                { GLFW_KEY_KP_1, DJV_TEXT("KeyPad1") },
                { GLFW_KEY_KP_2, DJV_TEXT("KeyPad2") },
                { GLFW_KEY_KP_3, DJV_TEXT("KeyPad3") },
                { GLFW_KEY_KP_4, DJV_TEXT("KeyPad4") },
                { GLFW_KEY_KP_5, DJV_TEXT("KeyPad5") },
                { GLFW_KEY_KP_6, DJV_TEXT("KeyPad6") },
                { GLFW_KEY_KP_7, DJV_TEXT("KeyPad7") },
                { GLFW_KEY_KP_8, DJV_TEXT("KeyPad8") },
                { GLFW_KEY_KP_9, DJV_TEXT("KeyPad9") },
                { GLFW_KEY_KP_DECIMAL, DJV_TEXT("KeyPadDecimal") },
                { GLFW_KEY_KP_DIVIDE, DJV_TEXT("KeyPadDivide") },
                { GLFW_KEY_KP_MULTIPLY, DJV_TEXT("KeyPadMultiply") },
                { GLFW_KEY_KP_SUBTRACT, DJV_TEXT("KeyPadSubstract") },
                { GLFW_KEY_KP_ADD, DJV_TEXT("KeyPadAdd") },
                { GLFW_KEY_KP_ENTER, DJV_TEXT("KeyPadEnter") },
                { GLFW_KEY_KP_EQUAL, DJV_TEXT("KeyPadEqual") },
                { GLFW_KEY_LEFT_SHIFT, DJV_TEXT("LeftShift") },
                { GLFW_KEY_LEFT_CONTROL, DJV_TEXT("LeftControl") },
                { GLFW_KEY_LEFT_ALT, DJV_TEXT("LeftAlt") },
                { GLFW_KEY_LEFT_SUPER, DJV_TEXT("LeftSuper") },
                { GLFW_KEY_RIGHT_SHIFT, DJV_TEXT("RightShift") },
                { GLFW_KEY_RIGHT_CONTROL, DJV_TEXT("RightControl") },
                { GLFW_KEY_RIGHT_ALT, DJV_TEXT("RightAlt") },
                { GLFW_KEY_RIGHT_SUPER, DJV_TEXT("RightSuper") },
                { GLFW_KEY_MENU, DJV_TEXT("Menu") },
            };
            static const std::string empty;
            const auto i = data.find(key);
            return i != data.end() ? i->second : empty;
        }

        const std::string & Shortcut::getModifierString(int key)
        {
            static const std::map<int, std::string> data =
            {
                { GLFW_MOD_SHIFT, DJV_TEXT("Shift") },
                { GLFW_MOD_CONTROL, DJV_TEXT("Ctrl") },
                { GLFW_MOD_ALT, DJV_TEXT("Alt") },
                { GLFW_MOD_SUPER, DJV_TEXT("Super") },
                { GLFW_MOD_CAPS_LOCK, DJV_TEXT("CapsLock") },
                { GLFW_MOD_NUM_LOCK, DJV_TEXT("NumLock") }
            };
            static const std::string empty;
            const auto i = data.find(key);
            return i != data.end() ? i->second : empty;
        }

        std::string Shortcut::getText(int key, int keyModifiers, const std::shared_ptr<TextSystem> & textSystem)
        {
            std::vector<std::string> out;
            if (keyModifiers & GLFW_MOD_SHIFT)
            {
                out.push_back(textSystem->getText(getModifierString(GLFW_MOD_SHIFT)));
            }
            if (keyModifiers & GLFW_MOD_CONTROL)
            {
                out.push_back(textSystem->getText(getModifierString(GLFW_MOD_CONTROL)));
            }
            if (keyModifiers & GLFW_MOD_ALT)
            {
                out.push_back(textSystem->getText(getModifierString(GLFW_MOD_ALT)));
            }
            if (keyModifiers & GLFW_MOD_SUPER)
            {
                out.push_back(textSystem->getText(getModifierString(GLFW_MOD_SUPER)));
            }
            if (keyModifiers & GLFW_MOD_CAPS_LOCK)
            {
                out.push_back(textSystem->getText(getModifierString(GLFW_MOD_CAPS_LOCK)));
            }
            if (keyModifiers & GLFW_MOD_NUM_LOCK)
            {
                out.push_back(textSystem->getText(getModifierString(GLFW_MOD_NUM_LOCK)));
            }
            out.push_back(textSystem->getText(getKeyString(key)));
            return String::join(out, "+");
        }

    } // namespace UI
} // namespace djv
