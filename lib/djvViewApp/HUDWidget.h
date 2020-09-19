// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/View.h>

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the heads up display (HUD) widget.
        class HUDWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(HUDWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            HUDWidget();

        public:
            ~HUDWidget() override;

            static std::shared_ptr<HUDWidget> create(const std::shared_ptr<System::Context>&);

            void setHUDFrame(const Math::BBox2f&);
            void setHUDData(const HUDData&);
            void setHUDOptions(const HUDOptions&);

        protected:
            void _paintEvent(System::Event::Paint&) override;

            void _initEvent(System::Event::Init &) override;
            void _updateEvent(System::Event::Update&) override;

        private:
            void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

