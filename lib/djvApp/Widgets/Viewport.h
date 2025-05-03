// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlTimelineUI/Viewport.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Viewport.
        class Viewport : public tl::timelineui::Viewport
        {
            DTK_NON_COPYABLE(Viewport);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            Viewport();

        public:
            virtual ~Viewport();

            static std::shared_ptr<Viewport> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setPlayer(const std::shared_ptr<tl::timeline::Player>&) override;

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;
            void mouseMoveEvent(dtk::MouseMoveEvent&) override;
            void mousePressEvent(dtk::MouseClickEvent&) override;
            void mouseReleaseEvent(dtk::MouseClickEvent&) override;

        private:
            void _videoDataUpdate();
            void _hudUpdate();

            DTK_PRIVATE();
        };
    }
}

