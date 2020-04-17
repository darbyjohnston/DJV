// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MediaWidget.h>

#include <djvUI/LineEditBase.h>
#include <djvUI/NumericEdit.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Widget.h>

#include <djvCore/Frame.h>
#include <djvCore/Speed.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        class PointerWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(PointerWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            PointerWidget()
            {}

        public:
            static std::shared_ptr<PointerWidget> create(const std::shared_ptr<Core::Context>&);

            void setHoverCallback(const std::function<void(PointerData)>&);
            void setDragCallback(const std::function<void(PointerData)>&);
            void setScrollCallback(const std::function<void(const glm::vec2&)>&);

        protected:
            void _pointerEnterEvent(Core::Event::PointerEnter&) override;
            void _pointerLeaveEvent(Core::Event::PointerLeave&) override;
            void _pointerMoveEvent(Core::Event::PointerMove&) override;
            void _buttonPressEvent(Core::Event::ButtonPress&) override;
            void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;
            void _scrollEvent(Core::Event::Scroll&) override;

        private:
            void _doHoverCallback(const PointerData&);
            void _doDragCallback(const PointerData&);

            uint32_t _pressedID = Core::Event::invalidID;
            std::map<int, bool> _buttons;
            std::function<void(PointerData)> _hoverCallback;
            std::function<void(PointerData)> _dragCallback;
            std::function<void(const glm::vec2&)> _scrollCallback;
        };

        class FrameWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(FrameWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            FrameWidget()
            {}

        public:
            static std::shared_ptr<FrameWidget> create(const std::shared_ptr<Core::Context>&);

            void setSequence(const Core::Frame::Sequence&);
            void setSpeed(const Core::Time::Speed&);
            void setFrame(const Core::Frame::Index);
            void setCallback(const std::function<void(Core::Frame::Index)>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _paintEvent(Core::Event::Paint&) override;

        private:
            void _setFrame(Core::Frame::Index);
            void _widgetUpdate();

            Core::Time::Units _timeUnits = Core::Time::Units::First;
            Core::Frame::Sequence _sequence;
            Core::Time::Speed _speed;
            Core::Frame::Index _index = 0;
            std::shared_ptr<UI::LineEditBase> _lineEditBase;
            std::shared_ptr<UI::NumericEditButtons> _buttons;
            std::shared_ptr<UI::HorizontalLayout> _layout;
            std::function<void(Core::Frame::Index)> _callback;
            std::map<std::string, std::shared_ptr<Core::ValueObserver<bool> > > _actionObservers;
            std::shared_ptr<Core::ValueObserver<Core::Time::Units> > _timeUnitsObserver;
        };

    } // namespace ViewApp
} // namespace djv

