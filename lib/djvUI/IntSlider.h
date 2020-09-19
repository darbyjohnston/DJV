// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/INumericWidget.h>
#include <djvUI/NumericSlider.h>

#include <djvMath/Range.h>

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
            void _init(Orientation, const std::shared_ptr<System::Context>&);
            BasicIntSlider();

        public:
            ~BasicIntSlider() override;

            static std::shared_ptr<BasicIntSlider> create(Orientation, const std::shared_ptr<System::Context>&);

            void setModel(const std::shared_ptr<Math::INumericValueModel<int> >&) override;

        protected:
            void _pointerMove(float) override;
            void _buttonPress(float) override;
            void _buttonRelease() override;
            bool _keyPress(int) override;
            void _scroll(float) override;
            void _valueUpdate() override;
        
            void _paintEvent(System::Event::Paint&) override;

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
            void _init(const std::shared_ptr<System::Context>&);
            IntSlider();

        public:
            ~IntSlider() override;

            static std::shared_ptr<IntSlider> create(const std::shared_ptr<System::Context>&);

            void setModel(const std::shared_ptr<Math::INumericValueModel<int> >&);

            Math::IntRange getRange() const;
            void setRange(const Math::IntRange&);

            int getValue() const;
            void setValue(int);
            void setValueCallback(const std::function<void(int)>&);
            
            int getDefault() const;
            void setDefault(int);
            void setDefaultVisible(bool);
            void resetValue();

            int getSmallIncrement() const;
            int getLargeIncrement() const;
            void setSmallIncrement(int);
            void setLargeIncrement(int);

            const Core::Time::Duration& getDelay() const;
            void setDelay(const Core::Time::Duration&);

            const std::shared_ptr<Math::INumericValueModel<int> >& getModel() const;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;
            
        private:
            void _modelUpdate();
            void _widgetUpdate();
            
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
