//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvUI/Widget.h>

#include <chrono>

namespace djv
{
    namespace UI
    {
        //! This class provides the base functionality for numeric slider widgets.
        class NumericSlider : public Widget
        {
        protected:
            void _init(Orientation, const std::shared_ptr<Core::Context>&);
            NumericSlider();

        public:
            virtual ~NumericSlider() = 0;

            Orientation getOrientation() const;
            
            const Core::Time::Duration& getDelay() const;
            void setDelay(const Core::Time::Duration&);

            bool acceptFocus(TextFocusDirection) override;

        protected:
            float _getHandleWidth() const;
            
            void _paint(float value, float pos);
            
            virtual void _pointerMove(float) = 0;
            virtual void _buttonPress(float) = 0;
            virtual void _buttonRelease() = 0;
            virtual bool _keyPress(int) = 0;
            virtual void _scroll(float) = 0;
            virtual void _valueUpdate() = 0;
            
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _pointerEnterEvent(Core::Event::PointerEnter &) override;
            void _pointerLeaveEvent(Core::Event::PointerLeave &) override;
            void _pointerMoveEvent(Core::Event::PointerMove &) override;
            void _buttonPressEvent(Core::Event::ButtonPress &) override;
            void _buttonReleaseEvent(Core::Event::ButtonRelease &) override;
            void _keyPressEvent(Core::Event::KeyPress&) override;
            void _textFocusEvent(Core::Event::TextFocus&) override;
            void _textFocusLostEvent(Core::Event::TextFocusLost&) override;
            void _scrollEvent(Core::Event::Scroll&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _resetTimer();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
