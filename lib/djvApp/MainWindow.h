// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Models/SettingsModel.h>

#include <tlTimelineUI/Window.h>

#include <tlTimeline/Player.h>

namespace tl
{
    namespace timelineui
    {
        class TimelineWidget;
    }
}

namespace djv
{
    namespace app
    {
        class App;
        class Viewport;

        //! Main window.
        class MainWindow : public tl::timelineui::Window
        {
            DTK_NON_COPYABLE(MainWindow);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&);

            MainWindow();

        public:
            ~MainWindow();

            //! Create a new main window.
            static std::shared_ptr<MainWindow> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&);

            //! Get the viewport.
            const std::shared_ptr<Viewport>& getViewport() const;

            //! Get the timeline widget.
            const std::shared_ptr<tl::timelineui::TimelineWidget>& getTimelineWidget() const;

            //! Focus the current frame widget.
            void focusCurrentFrame();

            //! Show the about dialog.
            void showAboutDialog();

            void setGeometry(const dtk::Box2I&) override;
            void keyPressEvent(dtk::KeyEvent&) override;
            void keyReleaseEvent(dtk::KeyEvent&) override;

        protected:
            void _drop(const std::vector<std::string>&) override;

        private:
            void _playerUpdate(const std::shared_ptr<tl::timeline::Player>&);
            void _settingsUpdate(const WindowSettings&);
            void _settingsUpdate(const TimelineSettings&);

            DTK_PRIVATE();
        };
    }
}
