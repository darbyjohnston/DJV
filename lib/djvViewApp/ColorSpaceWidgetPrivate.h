// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the color space configurations widget.
        class ColorSpaceConfigsWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ColorSpaceConfigsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorSpaceConfigsWidget();

        public:
            ~ColorSpaceConfigsWidget() override;

            static std::shared_ptr<ColorSpaceConfigsWidget> create(const std::shared_ptr<Core::Context>&);

            void setBackCallback(const std::function<void()>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides the color space displays widget.
        class ColorSpaceDisplaysWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ColorSpaceDisplaysWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorSpaceDisplaysWidget();

        public:
            ~ColorSpaceDisplaysWidget() override;

            static std::shared_ptr<ColorSpaceDisplaysWidget> create(const std::shared_ptr<Core::Context>&);

            void setBackCallback(const std::function<void()>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides the color space views widget.
        class ColorSpaceViewsWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ColorSpaceViewsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorSpaceViewsWidget();

        public:
            ~ColorSpaceViewsWidget() override;

            static std::shared_ptr<ColorSpaceViewsWidget> create(const std::shared_ptr<Core::Context>&);

            void setBackCallback(const std::function<void()>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides the image color spaces widget.
        class ImageColorSpacesWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ImageColorSpacesWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ImageColorSpacesWidget();

        public:
            ~ImageColorSpacesWidget() override;

            static std::shared_ptr<ImageColorSpacesWidget> create(const std::shared_ptr<Core::Context>&);

            void setImage(const std::string&);

            void setBackCallback(const std::function<void()>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

