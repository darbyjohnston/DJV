//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvViewLib/Enum.h>

#include <djvUI/Widget.h>

#include <djvCore/Time.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewLib
    {
        class TimelineSlider : public UI::Widget
        {
            DJV_NON_COPYABLE(TimelineSlider);

        protected:
            void _init(Core::Context *);
            TimelineSlider();

        public:
            ~TimelineSlider() override;

            static std::shared_ptr<TimelineSlider> create(Core::Context *);

            std::shared_ptr<Core::IValueSubject<Core::Time::Timestamp> > observeCurrentTime() const;

            void setDuration(Core::Time::Duration);
            void setCurrentTime(Core::Time::Timestamp);

        protected:
			void _styleEvent(Core::Event::Style &) override;
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _paintEvent(Core::Event::Paint &) override;
            void _pointerEnterEvent(Core::Event::PointerEnter &) override;
            void _pointerLeaveEvent(Core::Event::PointerLeave &) override;
            void _pointerMoveEvent(Core::Event::PointerMove &) override;
            void _buttonPressEvent(Core::Event::ButtonPress &) override;
            void _buttonReleaseEvent(Core::Event::ButtonRelease &) override;

			void _updateEvent(Core::Event::Update &) override;

        private:
            Core::Time::Timestamp _posToTime(int) const;
            Core::BBox2f _getHandleGeometry() const;
			void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewLib
} // namespace djv
