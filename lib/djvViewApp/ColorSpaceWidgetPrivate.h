// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the main color space widget.
        class ColorSpaceMainWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ColorSpaceMainWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorSpaceMainWidget();

        public:
            ~ColorSpaceMainWidget() override;

            static std::shared_ptr<ColorSpaceMainWidget> create(const std::shared_ptr<Core::Context>&);
            
            void setEditConfigsCallback(const std::function<void()>&);
            void setFileFormatsCallback(const std::function<void()>&);

        protected:
            void _initLayoutEvent(Core::Event::InitLayout&) override;
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides a widget for editing the list of color space
        //! configurations.
        class ColorSpaceEditConfigsWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ColorSpaceEditConfigsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorSpaceEditConfigsWidget();

        public:
            ~ColorSpaceEditConfigsWidget() override;

            static std::shared_ptr<ColorSpaceEditConfigsWidget> create(const std::shared_ptr<Core::Context>&);

            void setBackCallback(const std::function<void()>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides a widget for editing the file format color
        //! space associations.
        class ColorSpaceFileFormatsWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ColorSpaceFileFormatsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorSpaceFileFormatsWidget();

        public:
            ~ColorSpaceFileFormatsWidget() override;

            static std::shared_ptr<ColorSpaceFileFormatsWidget> create(const std::shared_ptr<Core::Context>&);

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

