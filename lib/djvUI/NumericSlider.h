// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
