// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        class LabelSizeGroup;
    
    } // namespace UI

    namespace ViewApp
    {
        //! This class provides the color space configuration widget.
        class ColorSpaceConfigWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ColorSpaceConfigWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorSpaceConfigWidget();

        public:
            ~ColorSpaceConfigWidget() override;

            static std::shared_ptr<ColorSpaceConfigWidget> create(const std::shared_ptr<Core::Context>&);

            void setSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides the color space display widget.
        class ColorSpaceDisplayWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ColorSpaceDisplayWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorSpaceDisplayWidget();

        public:
            ~ColorSpaceDisplayWidget() override;

            static std::shared_ptr<ColorSpaceDisplayWidget> create(const std::shared_ptr<Core::Context>&);

            void setSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides the color space image widget.
        class ColorSpaceImageWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ColorSpaceImageWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorSpaceImageWidget();

        public:
            ~ColorSpaceImageWidget() override;

            static std::shared_ptr<ColorSpaceImageWidget> create(const std::shared_ptr<Core::Context>&);

            void setSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _valueUpdate();
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

