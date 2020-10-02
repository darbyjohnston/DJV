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
        namespace Numeric
        {
            //! This class provides the base functionality for numeric slider widgets.
            class Slider : public Widget
            {
            protected:
                void _init(Orientation, const std::shared_ptr<System::Context>&);
                Slider();

            public:
                virtual ~Slider() = 0;

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

                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _pointerEnterEvent(System::Event::PointerEnter&) override;
                void _pointerLeaveEvent(System::Event::PointerLeave&) override;
                void _pointerMoveEvent(System::Event::PointerMove&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _buttonReleaseEvent(System::Event::ButtonRelease&) override;
                void _keyPressEvent(System::Event::KeyPress&) override;
                void _textFocusEvent(System::Event::TextFocus&) override;
                void _textFocusLostEvent(System::Event::TextFocusLost&) override;
                void _scrollEvent(System::Event::Scroll&) override;

                void _initEvent(System::Event::Init&) override;

            private:
                void _resetTimer();

                DJV_PRIVATE();
            };

        } // namespace Numeric
    } // namespace UI
} // namespace djv
