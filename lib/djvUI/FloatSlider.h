// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
            void _scroll(float) override;
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

            float getSmallIncrement() const;
            float getLargeIncrement() const;
            void setSmallIncrement(float);
            void setLargeIncrement(float);

            const Core::Time::Duration& getDelay() const;
            void setDelay(const Core::Time::Duration&);

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
