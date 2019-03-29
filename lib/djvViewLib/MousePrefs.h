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

#pragma once

#include <djvViewLib/AbstractPrefs.h>
#include <djvViewLib/Enum.h>

#include <QMap>

namespace djv
{
    namespace ViewLib
    {
        //! This struct provdes a mouse button action.
        struct MouseButtonAction
        {
            MouseButtonAction();
            MouseButtonAction(
                Enum::MOUSE_BUTTON,
                Enum::KEYBOARD_MODIFIER,
                Enum::MOUSE_BUTTON_ACTION);
            
            Enum::MOUSE_BUTTON button = Enum::MOUSE_BUTTON_NONE;
            Enum::KEYBOARD_MODIFIER modifier = Enum::KEYBOARD_MODIFIER_NONE;
            Enum::MOUSE_BUTTON_ACTION action = Enum::MOUSE_BUTTON_ACTION_NONE;

            static const QStringList & labels();

            bool operator == (const MouseButtonAction &) const;
        };

        //! This struct provdes a mouse wheel action.
        struct MouseWheelAction
        {
            MouseWheelAction();
            MouseWheelAction(
                Enum::KEYBOARD_MODIFIER,
                Enum::MOUSE_WHEEL_ACTION);

            Enum::KEYBOARD_MODIFIER modifier = Enum::KEYBOARD_MODIFIER_NONE;
            Enum::MOUSE_WHEEL_ACTION action = Enum::MOUSE_WHEEL_ACTION_NONE;

            static const QStringList & labels();

            bool operator == (const MouseWheelAction &) const;
        };

        //! This class provides the mouse preferences.
        class MousePrefs : public AbstractPrefs
        {
            Q_OBJECT

        public:
            explicit MousePrefs(const QPointer<ViewContext> &, QObject * parent = nullptr);
            ~MousePrefs() override;

            //! Get the default mouse button actions.
            static const QVector<MouseButtonAction> & mouseButtonActionsDefault();

            //! Get the mouse button actions.
            const QVector<MouseButtonAction> & mouseButtonActions() const;

            //! Get the default mouse wheel action.
            static const QVector<MouseWheelAction> & mouseWheelActionsDefault();

            //! Get the mouse wheel action.
            const QVector<MouseWheelAction> & mouseWheelActions() const;

        public Q_SLOTS:
            //! Set the mouse button actions.
            void setMouseButtonActions(const QVector<MouseButtonAction> &);

            //! Set the mouse wheel actions.
            void setMouseWheelActions(const QVector<MouseWheelAction> &);

        private:
            QVector<MouseButtonAction> _mouseButtonActions;
            QVector<MouseWheelAction> _mouseWheelActions;
        };

    } // namespace ViewLib

    QStringList & operator >> (QStringList &, QVector<ViewLib::MouseButtonAction> &);
    QStringList & operator >> (QStringList &, QVector<ViewLib::MouseWheelAction> &);
    QStringList & operator << (QStringList &, const QVector<ViewLib::MouseButtonAction> &);
    QStringList & operator << (QStringList &, const QVector<ViewLib::MouseWheelAction> &);

} // namespace djv
