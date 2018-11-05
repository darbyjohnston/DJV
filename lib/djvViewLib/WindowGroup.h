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

#include <djvViewLib/AbstractGroup.h>
#include <djvViewLib/Enum.h>

#include <QMap>

#include <memory>

class QAction;

namespace djv
{
    namespace ViewLib
    {
        //! This class provides the window group. The window group encapsulates all
        //! of the functionality relating to main windows such as whether the window
        //! is full screen, which tool bars are visible, etc.
        class WindowGroup : public AbstractGroup
        {
            Q_OBJECT

        public:
            WindowGroup(
                const QPointer<WindowGroup> & copy,
                const QPointer<MainWindow> &,
                const QPointer<ViewContext> &);
            ~WindowGroup() override;

            //! Get whether full screen is enabled.    
            bool hasFullScreen() const;

            //! Get the user interface visibility.
            bool isUIVisible() const;

            //! Get the UI component visibility.
            const QMap<Enum::UI_COMPONENT, bool> & uiComponentVisible() const;

            //! Get whether the controls window is enabled.
            bool hasControlsWindow() const;

            QPointer<QMenu> createMenu() const override;
            QPointer<QToolBar> createToolBar() const override;

        public Q_SLOTS:
            //! Set whether full screen is enabled.
            void setFullScreen(bool);

            //! Set the user interface visibility.
            void setUIVisible(bool);

            //! Set the UI component visibility.
            void setUIComponentVisible(const QMap<djv::ViewLib::Enum::UI_COMPONENT, bool> &);

            //! Set whether the controls window is enabled.
            void setControlsWindow(bool);

        Q_SIGNALS:
            //! This signal is emitted when full screen is changed.
            void fullScreenChanged(bool);

            //! This signal is emitted when the user interface visibility is changed.
            void uiVisibleChanged(bool);

            //! This signal is emitted when the UI component visibility is changed.
            void uiComponentVisibleChanged(const QMap<djv::ViewLib::Enum::UI_COMPONENT, bool> &);

            //! This signal is emitted when the controls window state is changed.
            void controlsWindowChanged(bool);

        private Q_SLOTS:
            void newCallback();
            void duplicateCallback();
            void closeCallback();
            void fitCallback();
            void uiComponentVisibleCallback(QAction *);

            void update();

        private:
            DJV_PRIVATE_COPY(WindowGroup);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
