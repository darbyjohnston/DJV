// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MediaWidget.h>

#include <djvMath/FrameNumber.h>

namespace djv
{
    namespace Math
    {
        class Rational;
    
    } // namespace Math

    namespace ViewApp
    {
        class TitleBar : public UI::Widget
        {
            DJV_NON_COPYABLE(TitleBar);

        protected:
            void _init(
                const std::string& text,
                const std::string& tooltip,
                const std::shared_ptr<System::Context>&);
            TitleBar();

        public:
            ~TitleBar() override;

            static std::shared_ptr<TitleBar> create(
                const std::string& text,
                const std::string& toolTip,
                const std::shared_ptr<System::Context>&);

            void setActive(bool);
            void setMaximize(float);

            void setMaximizeCallback(const std::function<void(void)>&);
            void setCloseCallback(const std::function<void(void)>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        class PointerWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(PointerWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            PointerWidget();

        public:
            static std::shared_ptr<PointerWidget> create(const std::shared_ptr<System::Context>&);

            void setHoverCallback(const std::function<void(const PointerData&)>&);
            void setDragCallback(const std::function<void(const PointerData&)>&);
            void setScrollCallback(const std::function<void(const ScrollData&)>&);

        protected:
            void _pointerEnterEvent(System::Event::PointerEnter&) override;
            void _pointerLeaveEvent(System::Event::PointerLeave&) override;
            void _pointerMoveEvent(System::Event::PointerMove&) override;
            void _buttonPressEvent(System::Event::ButtonPress&) override;
            void _buttonReleaseEvent(System::Event::ButtonRelease&) override;
            void _keyPressEvent(System::Event::KeyPress&) override;
            void _keyReleaseEvent(System::Event::KeyRelease&) override;
            void _scrollEvent(System::Event::Scroll&) override;

        private:
            void _doHoverCallback(const PointerData&);
            void _doDragCallback(const PointerData&);

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

