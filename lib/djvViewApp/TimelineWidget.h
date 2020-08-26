// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        class Media;

        //! This class provides a timeline widget.
        class TimelineWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(TimelineWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            TimelineWidget();

        public:
            ~TimelineWidget() override;

            static std::shared_ptr<TimelineWidget> create(const std::shared_ptr<Core::Context>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();
            void _speedUpdate();
            void _realSpeedUpdate();
            void _audioUpdate();
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv
