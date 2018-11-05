//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

namespace djv
{
    namespace ViewLib
    {
        //! This class provides the window group preferences.
        class WindowPrefs : public AbstractPrefs
        {
            Q_OBJECT

        public:
            explicit WindowPrefs(const QPointer<ViewContext> &, QObject * parent = nullptr);
            ~WindowPrefs() override;

            //! Get the default for whether to automatically fit the window to the image.
            static bool autoFitDefault();

            //! Get whether to automatically fit the window to the image.
            bool hasAutoFit() const;

            //! Get the default maximum view size.
            static Enum::VIEW_MAX viewMaxDefault();

            //! Get the maximum view size.
            Enum::VIEW_MAX viewMax() const;

            //! Get the default user specified maximum view size.
            static const glm::ivec2 & viewMaxUserDefault();

            //! Get the user specified maximum view size.
            const glm::ivec2 & viewMaxUser() const;

            //! Get the default full screen UI option.
            static Enum::FULL_SCREEN_UI fullScreenUIDefault();

            //! Get the full screen UI option.
            Enum::FULL_SCREEN_UI fullScreenUI() const;

            //! Get the default UI component visiblity.
            static QVector<bool> uiComponentVisibleDefault();

            //! Get the UI component visibility.
            const QVector<bool> & uiComponentVisible() const;

            //! Get the default controls window enabled state.
            static bool controlsWindowDefault();

            //! Get whether the controls window is enabled.
            bool hasControlsWindow() const;

        public Q_SLOTS:
            //! Set whether to automatically fit the window to the image.
            void setAutoFit(bool);

            //! Set the maximum view size.
            void setViewMax(djv::ViewLib::Enum::VIEW_MAX);

            //! Set the user specified maximum view size.
            void setViewMaxUser(const glm::ivec2 &);

            //! Set the full screen UI option.
            void setFullScreenUI(djv::ViewLib::Enum::FULL_SCREEN_UI);

            //! Set the UI component visibility.
            void setUIComponentVisible(const QVector<bool> &);

            //! Set whether the controls window is enabled.
            void setControlsWindow(bool);

        Q_SIGNALS:
            //! This signal is emitted when automatically fitting the window to the
            //! image is changed.
            void autoFitChanged(bool);

            //! This signal is emitted when the maximum view size is changed.
            void viewMaxChanged(djv::ViewLib::Enum::VIEW_MAX);

            //! This signal is emitted when the user specified maximum view size is changed.
            void viewMaxUserChanged(const glm::ivec2 &);

            //! This signal is emitted when the full screen UI option is changed.
            void fullScreenUIChanged(djv::ViewLib::Enum::FULL_SCREEN_UI);

            //! This signal is emitted when the tool bar visibility is changed.
            void uiComponentVisibleChanged(const QVector<bool> &);

            //! This signal is emitted when the controls window state is changed.
            void controlsWindowChanged(bool);

        private:
            bool                 _autoFit;
            Enum::VIEW_MAX       _viewMax;
            glm::ivec2           _viewMaxUser = glm::ivec2(0, 0);
            Enum::FULL_SCREEN_UI _fullScreenUI;
            QVector<bool>        _uiComponentVisible;
            bool                 _controlsWindow;
        };

    } // namespace ViewLib
} // namespace djv
