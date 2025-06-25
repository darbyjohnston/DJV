// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlTimelineUI/Window.h>

#include <tlTimeline/Player.h>

#include <feather-tk/ui/Window.h>

namespace tl
{
    namespace timelineui
    {
        class Viewport;
    }
}

namespace djv
{
    namespace app
    {
        class App;

        //! Secondary window.
        class SecondaryWindow : public tl::timelineui::Window
        {
            FEATHER_TK_NON_COPYABLE(SecondaryWindow);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<feather_tk::Window>& shared);

            SecondaryWindow();

        public:
            virtual ~SecondaryWindow();

            static std::shared_ptr<SecondaryWindow> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<feather_tk::Window>& shared = nullptr);

            //! Get the viewport.
            const std::shared_ptr<tl::timelineui::Viewport>& getViewport() const;

            //! Set the view.
            void setView(
                const feather_tk::V2I& pos,
                double          zoom,
                bool            frame);

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
