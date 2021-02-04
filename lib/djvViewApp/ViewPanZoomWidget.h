// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the view pan and zoom widget.
        class ViewPanZoomWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ViewPanZoomWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ViewPanZoomWidget();

        public:
            ~ViewPanZoomWidget() override;

            static std::shared_ptr<ViewPanZoomWidget> create(const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            void _setPos(const glm::vec2&);
            void _setZoom(float);
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

