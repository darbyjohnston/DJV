// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
            void _scroll(float) override;
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

            const Core::Time::Duration& getDelay() const;
            void setDelay(const Core::Time::Duration&);

            const std::shared_ptr<Core::INumericValueModel<int> >& getModel() const;

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
