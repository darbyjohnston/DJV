//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include <djvViewApp/MediaWidget.h>

#include <djvUI/LineEdit.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Widget.h>

#include <djvCore/Time.h>
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

        protected:
            void _pointerEnterEvent(Core::Event::PointerEnter&) override;
            void _pointerLeaveEvent(Core::Event::PointerLeave&) override;
            void _pointerMoveEvent(Core::Event::PointerMove&) override;
            void _buttonPressEvent(Core::Event::ButtonPress&) override;
            void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;

        private:
            uint32_t _pressedID = Core::Event::InvalidID;
            std::map<int, bool> _buttons;
            std::function<void(PointerData)> _hoverCallback;
            std::function<void(PointerData)> _dragCallback;
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

        private:
            void _setFrame(Core::Frame::Index);
            void _widgetUpdate();

            AV::TimeUnits _timeUnits = AV::TimeUnits::First;
            Core::Frame::Sequence _sequence;
            Core::Time::Speed _speed;
            Core::Frame::Index _index = 0;
            std::shared_ptr<UI::LineEdit> _lineEdit;
            std::function<void(Core::Frame::Index)> _callback;
            std::map<std::string, std::shared_ptr<Core::ValueObserver<bool> > > _actionObservers;
            std::shared_ptr<Core::ValueObserver<AV::TimeUnits> > _timeUnitsObserver;
        };

    } // namespace ViewApp
} // namespace djv

