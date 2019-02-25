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

#include <djvUI/Widget.h>

namespace djv
{
    namespace Core
    {
        class FloatValueModel;

    } // namespace Core

    namespace UI
    {
        //! This namespace provides slider widgets.
        namespace Slider
        {
            //! This class provides a floating-point value slider.
            class FloatValue : public Widget
            {
                DJV_NON_COPYABLE(FloatValue);

            protected:
                void _init(Orientation, Core::Context *);
                FloatValue();

            public:
                virtual ~FloatValue();

                static std::shared_ptr<FloatValue> create(Orientation, Core::Context *);

                const std::shared_ptr<Core::FloatValueModel> & getModel() const;
                void setModel(const std::shared_ptr<Core::FloatValueModel> &);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _paintEvent(Core::Event::Paint&) override;
                void _pointerEnterEvent(Core::Event::PointerEnter&) override;
                void _pointerLeaveEvent(Core::Event::PointerLeave&) override;
                void _pointerMoveEvent(Core::Event::PointerMove&) override;
                void _buttonPressEvent(Core::Event::ButtonPress&) override;
                void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;

            private:
                ColorRole _getColorRole() const;

                float _valueToPos(float) const;
                float _posToValue(float) const;

                DJV_PRIVATE();
            };

        } // namespace Slider

        typedef Slider::FloatValue FloatValueSlider;

    } // namespace UI
} // namespace djv
