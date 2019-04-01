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

#include <djvViewLib/MousePrefs.h>

#include <djvUI/Prefs.h>

#include <QCoreApplication>

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            const QVector<MouseButtonAction> mouseButtonActionsDefault =
            {
                { Enum::MOUSE_BUTTON_LEFT, Enum::KEYBOARD_MODIFIER_NONE, Enum::MOUSE_BUTTON_ACTION_PICK },
                { Enum::MOUSE_BUTTON_LEFT, Enum::KEYBOARD_MODIFIER_SHIFT, Enum::MOUSE_BUTTON_ACTION_VIEW_MOVE },
                { Enum::MOUSE_BUTTON_LEFT, Enum::KEYBOARD_MODIFIER_CTRL, Enum::MOUSE_BUTTON_ACTION_PLAYBACK_SHUTTLE },
                { Enum::MOUSE_BUTTON_MIDDLE, Enum::KEYBOARD_MODIFIER_NONE, Enum::MOUSE_BUTTON_ACTION_VIEW_MOVE },
                { Enum::MOUSE_BUTTON_MIDDLE, Enum::KEYBOARD_MODIFIER_CTRL, Enum::MOUSE_BUTTON_ACTION_VIEW_ZOOM_IN },
                { Enum::MOUSE_BUTTON_RIGHT, Enum::KEYBOARD_MODIFIER_NONE, Enum::MOUSE_BUTTON_ACTION_CONTEXT_MENU },
                { Enum::MOUSE_BUTTON_RIGHT, Enum::KEYBOARD_MODIFIER_CTRL, Enum::MOUSE_BUTTON_ACTION_VIEW_ZOOM_OUT }
            };

            const QVector<MouseWheelAction> mouseWheelActionsDefault =
            {
                { Enum::KEYBOARD_MODIFIER_NONE, Enum::MOUSE_WHEEL_ACTION_VIEW_ZOOM },
                { Enum::KEYBOARD_MODIFIER_SHIFT, Enum::MOUSE_WHEEL_ACTION_PLAYBACK_SHUTTLE, },
                { Enum::KEYBOARD_MODIFIER_CTRL, Enum::MOUSE_WHEEL_ACTION_PLAYBACK_SPEED }
            };

        } // namespace

        MouseButtonAction::MouseButtonAction()
        {}
        
        MouseButtonAction::MouseButtonAction(
            Enum::MOUSE_BUTTON button,
            Enum::KEYBOARD_MODIFIER modifier,
            Enum::MOUSE_BUTTON_ACTION action) :
            button(button),
            modifier(modifier),
            action(action)
        {}
        
        const QStringList & MouseButtonAction::labels()
        {
            static const QStringList data =
            {
                qApp->translate("djv::ViewLib::MouseButtonAction", "Button"),
                qApp->translate("djv::ViewLib::MouseButtonAction", "Keyboard Modifier"),
                qApp->translate("djv::ViewLib::MouseButtonAction", "Action")
            };
            return data;
        }

        bool MouseButtonAction::operator == (const MouseButtonAction & other) const
        {
            return button == other.button &&
                modifier == other.modifier &&
                action == other.action;
        }
        
        MouseWheelAction::MouseWheelAction()
        {}
        
        MouseWheelAction::MouseWheelAction(
            Enum::KEYBOARD_MODIFIER modifier,
            Enum::MOUSE_WHEEL_ACTION action) :
            modifier(modifier),
            action(action)
        {}

        const QStringList & MouseWheelAction::labels()
        {
            static const QStringList data =
            {
                qApp->translate("djv::ViewLib::MouseWheelAction", "Keyboard Modifier"),
                qApp->translate("djv::ViewLib::MouseWheelAction", "Action")
            };
            return data;
        }

        bool MouseWheelAction::operator == (const MouseWheelAction & other) const
        {
            return modifier == other.modifier &&
                action == other.action;
        }

        MousePrefs::MousePrefs(const QPointer<ViewContext> & context, QObject * parent) :
            AbstractPrefs(context, parent)
        {
            //DJV_DEBUG("MousePrefs::MousePrefs");
            UI::Prefs prefs("djv::ViewLib::MousePrefs");
            if (!prefs.get("mouseButtonActions", _mouseButtonActions))
            {
                _mouseButtonActions = mouseButtonActionsDefault;
            }
            if (!prefs.get("mouseWheelActions", _mouseWheelActions))
            {
                _mouseWheelActions = mouseWheelActionsDefault;
            }
        }

        MousePrefs::~MousePrefs()
        {
            //DJV_DEBUG("MousePrefs::~MousePrefs");
            UI::Prefs prefs("djv::ViewLib::MousePrefs");
            prefs.set("mouseButtonActions", _mouseButtonActions);
            prefs.set("mouseWheelActions", _mouseWheelActions);
        }

        const QVector<MouseButtonAction> & MousePrefs::mouseButtonActions() const
        {
            return _mouseButtonActions;
        }

        const QVector<MouseWheelAction> & MousePrefs::mouseWheelActions() const
        {
            return _mouseWheelActions;
        }

        void MousePrefs::reset()
        {
            setMouseButtonActions(mouseButtonActionsDefault);
            setMouseWheelActions(mouseWheelActionsDefault);
        }

        void MousePrefs::setMouseButtonActions(const QVector<MouseButtonAction> & value)
        {
            if (value == _mouseButtonActions)
                return;
            _mouseButtonActions = value;
            Q_EMIT prefChanged();
        }

        void MousePrefs::setMouseWheelActions(const QVector<MouseWheelAction> & value)
        {
            if (value == _mouseWheelActions)
                return;
            _mouseWheelActions = value;
            Q_EMIT prefChanged();
        }

    } // namespace ViewLib

    QStringList & operator >> (QStringList & in, QVector<ViewLib::MouseButtonAction> & out)
    {
        while (in.count())
        {
            ViewLib::MouseButtonAction action;
            in >> action.button;
            if (in.count())
            {
                in >> action.modifier;
                if (in.count())
                {
                    in >> action.action;
                    out.append(action);
                }
            }
        }
        return in;
    }

    QStringList & operator >> (QStringList & in, QVector<ViewLib::MouseWheelAction> & out)
    {
        while (in.count())
        {
            ViewLib::MouseWheelAction action;
            in >> action.modifier;
            if (in.count())
            {
                in >> action.action;
                out.append(action);
            }
        }
        return in;
    }

    QStringList & operator << (QStringList & out, const QVector<ViewLib::MouseButtonAction> & in)
    {
        Q_FOREACH(auto i, in)
        {
            out << i.button << i.modifier << i.action;
        }
        return out;
    }

    QStringList & operator << (QStringList & out, const QVector<ViewLib::MouseWheelAction> & in)
    {
        Q_FOREACH(auto i, in)
        {
            out << i.modifier << i.action;
        }
        return out;
    }

} // namespace djv
