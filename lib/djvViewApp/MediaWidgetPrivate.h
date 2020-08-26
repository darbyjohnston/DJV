// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MediaWidget.h>

namespace djv
{
    namespace ViewApp
    {
        class TitleBar : public UI::Widget
        {
            DJV_NON_COPYABLE(TitleBar);

        protected:
            void _init(
                const std::string& text,
                const std::string& tooltip,
                const std::shared_ptr<Core::Context>&);
            TitleBar();

        public:
            ~TitleBar() override;

            static std::shared_ptr<TitleBar> create(
                const std::string& text,
                const std::string& toolTip,
                const std::shared_ptr<Core::Context>&);

            void setActive(bool);
            void setMaximize(float);

            void setMaximizeCallback(const std::function<void(void)>&);
            void setCloseCallback(const std::function<void(void)>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        class PointerWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(PointerWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            PointerWidget();

        public:
            static std::shared_ptr<PointerWidget> create(const std::shared_ptr<Core::Context>&);

            void setHoverCallback(const std::function<void(const PointerData&)>&);
            void setDragCallback(const std::function<void(const PointerData&)>&);
            void setScrollCallback(const std::function<void(const ScrollData&)>&);

        protected:
            void _pointerEnterEvent(Core::Event::PointerEnter&) override;
            void _pointerLeaveEvent(Core::Event::PointerLeave&) override;
            void _pointerMoveEvent(Core::Event::PointerMove&) override;
            void _buttonPressEvent(Core::Event::ButtonPress&) override;
            void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;
            void _keyPressEvent(Core::Event::KeyPress&) override;
            void _keyReleaseEvent(Core::Event::KeyRelease&) override;
            void _scrollEvent(Core::Event::Scroll&) override;

        private:
            void _doHoverCallback(const PointerData&);
            void _doDragCallback(const PointerData&);

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

