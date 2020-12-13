// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace Image
    {
        class Color;

    } // namespace Image

    namespace ViewApp
    {
        struct GridOptions;

        //! This class provides the image controls widget.
        class ImageControlsWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ImageControlsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ImageControlsWidget();

        public:
            ~ImageControlsWidget() override;

            static std::shared_ptr<ImageControlsWidget> create(const std::shared_ptr<System::Context>&);

            std::map<std::string, bool> getBellowsState() const;

            void setBellowsState(const std::map<std::string, bool>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

