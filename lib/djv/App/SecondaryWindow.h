// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the DJV project.

#pragma once

#include <djv/App/Export.h>
#include <djv/Models/Export.h>

#include <tlRender/Timeline/Player.h>

#include <ftk/UI/Window.h>

namespace djv
{
    namespace ui
    {
        class Viewport;
    }

    namespace app
    {
        class App;

        //! Secondary window.
        class DJV_APP_API_TYPE SecondaryWindow : public ftk::Window
        {
            FTK_NON_COPYABLE(SecondaryWindow);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<ftk::Window>& shared,
                int screen,
                const ftk::Size2I& size);

            SecondaryWindow();

        public:
            DJV_APP_API virtual ~SecondaryWindow();

            //! The screen is an index into the monitors, or -1 for wherever
            //! the system puts a window. The size is the window's; on a
            //! screen of its own it is the screen's.
            DJV_APP_API static std::shared_ptr<SecondaryWindow> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<ftk::Window>& shared = nullptr,
                int screen = -1,
                const ftk::Size2I& size = ftk::Size2I(1280, 960));

            //! Get the viewport.
            DJV_APP_API const std::shared_ptr<ui::Viewport>& getViewport() const;

            //! Set the view.
            DJV_APP_API void setView(
                const ftk::V2I& pos,
                double          zoom,
                bool            frame);

            DJV_APP_API void keyPressEvent(ftk::KeyEvent&) override;
            DJV_APP_API void keyReleaseEvent(ftk::KeyEvent&) override;

        private:
            FTK_PRIVATE();
        };
    }
}
