// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <djvCore/INumericValueModel.h>
#include <djvCore/Range.h>
#include <djvCore/Time.h>
#include <djvCore/ValueObserver.h>

#include <chrono>

namespace djv
{
    namespace UI
    {
        //! This enumeration provides the numeric widget keyboard shortcuts.
        enum class NumericWidgetKey
        {
            None,
            Home,
            End,
            Up,
            Right,
            Down,
            Left,
            PageUp,
            PageDown
        };
        NumericWidgetKey fromGLFWKey(int);

        //! This class provides an interface for numeric widgets.
        template<typename T>
        class INumericWidget
        {
        public:
            virtual ~INumericWidget() = 0;

            const Core::Math::Range<T>& getRange() const;
            void setRange(const Core::Math::Range<T>&);

            T getValue() const;
            void setValue(T);
            void setValueCallback(const std::function<void(T, TextEditReason)>&);

            T getSmallIncrement() const;
            T getLargeIncrement() const;
            void setSmallIncrement(T);
            void setLargeIncrement(T);

            const std::shared_ptr<Core::INumericValueModel<T> >& getModel() const;
            virtual void setModel(const std::shared_ptr<Core::INumericValueModel<T> >&);

        protected:
            void _doCallback(TextEditReason);
            virtual bool _doKeyPress(NumericWidgetKey);
            virtual void _setIsMin(bool) {}
            virtual void _setIsMax(bool) {}

            std::shared_ptr<Core::INumericValueModel<T> > _model;

        private:
            std::shared_ptr<Core::ValueObserver<bool> > _isMinObserver;
            std::shared_ptr<Core::ValueObserver<bool> > _isMaxObserver;
            std::function<void(T, TextEditReason)> _callback;
        };

        //! This class provides an interface for numeric editor widgets.
        template<typename T>
        class INumericEdit : public INumericWidget<T>
        {
        public:
            virtual ~INumericEdit() = 0;
            
        protected:
            bool _doKeyPress(NumericWidgetKey) override;
        };

        //! This class provides an interface for numeric slider widgets.
        template<typename T>
        class INumericSlider : public INumericWidget<T>
        {
        public:
            virtual ~INumericSlider() = 0;

        protected:
            void _pointerMove(float, const Core::Time::Duration&);
            void _buttonPress(float, const Core::Time::Duration&);
            void _buttonRelease(const Core::Time::Duration&);
            void _valueUpdate();

            virtual float _valueToPos(T) const = 0;
            virtual T _posToValue(float) const = 0;

            T _value = static_cast<T>(0);
        };

    } // namespace UI
} // namespace djv

#include <djvUI/INumericWidgetInline.h>
