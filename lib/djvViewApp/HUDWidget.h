// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/HUD.h>

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
            void _init(const std::shared_ptr<Core::Context>&);
            HUDWidget();

        public:
            ~HUDWidget() override;

            static std::shared_ptr<HUDWidget> create(const std::shared_ptr<Core::Context>&);

            void setHUDFrame(const Core::BBox2f&);
            void setHUDData(const HUDData&);
            void setHUDOptions(const HUDOptions&);

        protected:
            void _paintEvent(Core::Event::Paint&) override;

            void _initEvent(Core::Event::Init &) override;
            void _updateEvent(Core::Event::Update&) override;

        private:
            void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

