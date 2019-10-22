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

#include <chrono>

namespace djv
{
    namespace UI
    {
        //! This class provides a basic slider widget for integer values.
        class BasicIntSlider : public INumericSlider<int>, public NumericSlider
        {
            DJV_NON_COPYABLE(BasicIntSlider);

        protected:
            void _init(Orientation, const std::shared_ptr<Core::Context>&);
            BasicIntSlider();

        public:
            ~BasicIntSlider() override;

            static std::shared_ptr<BasicIntSlider> create(Orientation, const std::shared_ptr<Core::Context>&);

            void setModel(const std::shared_ptr<Core::INumericValueModel<int> > &) override;

        protected:
            void _pointerMove(float) override;
            void _buttonPress(float) override;
            void _buttonRelease() override;
            bool _keyPress(int) override;
            void _valueUpdate() override;
        
            void _paintEvent(Core::Event::Paint &) override;

        private:
            float _valueToPos(int) const override;
            int _posToValue(float) const override;

            DJV_PRIVATE();
        };

        //! This class provides a slider widget for integer values.
        class IntSlider : public Widget
        {
            DJV_NON_COPYABLE(IntSlider);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            IntSlider();

        public:
            ~IntSlider() override;

            static std::shared_ptr<IntSlider> create(const std::shared_ptr<Core::Context>&);

            void setModel(const std::shared_ptr<Core::INumericValueModel<int> >&);

            Core::IntRange getRange() const;
            void setRange(const Core::IntRange &);

            int getValue() const;
            void setValue(int);
            void setValueCallback(const std::function<void(int)> &);
            
            float getDefault() const;
            void setDefault(float);
            void setDefaultVisible(bool);
            void resetValue();

            std::chrono::milliseconds getDelay() const;
            void setDelay(std::chrono::milliseconds);

            const std::shared_ptr<Core::INumericValueModel<int> >& getModel() const;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;

            void _textUpdateEvent(Core::Event::TextUpdate&) override;
            
        private:
            void _modelUpdate();
            void _widgetUpdate();
            
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
