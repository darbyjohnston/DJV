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

#include <djvUI/INumericWidget.h>
#include <djvUI/NumericSlider.h>

#include <djvCore/Range.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a basic slider widget for floating-point values.
        class BasicFloatSlider : public INumericSlider<float>, public NumericSlider
        {
            DJV_NON_COPYABLE(BasicFloatSlider);

        protected:
            void _init(Orientation, const std::shared_ptr<Core::Context>&);
            BasicFloatSlider();

        public:
            ~BasicFloatSlider() override;

            static std::shared_ptr<BasicFloatSlider> create(Orientation, const std::shared_ptr<Core::Context>&);

            void setModel(const std::shared_ptr<Core::INumericValueModel<float> > &) override;

        protected:
            void _pointerMove(float) override;
            void _buttonPress(float) override;
            void _buttonRelease() override;
            bool _keyPress(int) override;
            void _valueUpdate() override;

            void _paintEvent(Core::Event::Paint &) override;

        private:
            float _valueToPos(float) const override;
            float _posToValue(float) const override;

            DJV_PRIVATE();
        };

        //! This class provides a slider widget for floating-point values.
        class FloatSlider : public Widget
        {
            DJV_NON_COPYABLE(FloatSlider);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            FloatSlider();

        public:
            ~FloatSlider() override;

            static std::shared_ptr<FloatSlider> create(const std::shared_ptr<Core::Context>&);

            void setModel(const std::shared_ptr<Core::INumericValueModel<float> >&);

            Core::FloatRange getRange() const;
            void setRange(const Core::FloatRange &);

            float getValue() const;
            void setValue(float);
            void setValueCallback(const std::function<void(float)> &);
            
            float getDefault() const;
            void setDefault(float);
            void setDefaultVisible(bool);
            void resetValue();

            std::chrono::milliseconds getDelay() const;
            void setDelay(std::chrono::milliseconds);

            const std::shared_ptr<Core::INumericValueModel<float>>& getModel() const;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _modelUpdate();
            void _widgetUpdate();
            
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
