// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvMath/MathFunc.h>

namespace djv
{
    namespace Math
    {
        template<typename T>
        inline void INumericValueModel<T>::_init()
        {
            _range          = Core::Observer::ValueSubject<Math::Range<T> >::create();
            _value          = Core::Observer::ValueSubject<T>::create();
            _isMin          = Core::Observer::ValueSubject<bool>::create();
            _isMax          = Core::Observer::ValueSubject<bool>::create();
            _smallIncrement = Core::Observer::ValueSubject<T>::create();
            _largeIncrement = Core::Observer::ValueSubject<T>::create();
            _overflow       = Core::Observer::ValueSubject<NumericValueOverflow>::create(NumericValueOverflow::Clamp);
        }

        template<typename T>
        inline INumericValueModel<T>::INumericValueModel()
        {}

        template<typename T>
        inline INumericValueModel<T>::~INumericValueModel()
        {}

        template<typename T>
        inline std::shared_ptr<Core::Observer::IValueSubject<Math::Range<T> > > INumericValueModel<T>::observeRange() const
        {
            return _range;
        }

        template<typename T>
        inline void INumericValueModel<T>::setRange(const Math::Range<T>& value)
        {
            if (_range->setIfChanged(value))
            {
                setValue(_value->get());
            }
        }

        template<typename T>
        inline std::shared_ptr<Core::Observer::IValueSubject<T> > INumericValueModel<T>::observeValue() const
        {
            return _value;
        }

        template<typename T>
        inline std::shared_ptr<Core::Observer::IValueSubject<bool> > INumericValueModel<T>::observeIsMin() const
        {
            return _isMin;
        }

        template<typename T>
        inline std::shared_ptr<Core::Observer::IValueSubject<bool> > INumericValueModel<T>::observeIsMax() const
        {
            return _isMax;
        }

        template<typename T>
        inline void INumericValueModel<T>::setValue(T value)
        {
            const auto& range = _range->get();
            switch (_overflow->get())
            {
            case NumericValueOverflow::Clamp:
                value = Math::clamp(value, range.getMin(), range.getMax());
                break;
            case NumericValueOverflow::Wrap:
                if (range.getMin() != range.getMax())
                {
                    while (value > range.getMax())
                    {
                        value -= range.getMax();
                    }
                    while (value < range.getMin())
                    {
                        value += range.getMin();
                    }
                }
                break;
            default: break;
            }
            _value->setIfChanged(value);
            _isMin->setIfChanged(value == range.getMin());
            _isMax->setIfChanged(value == range.getMax());
        }

        template<typename T>
        inline void INumericValueModel<T>::setMin()
        {
            setValue(_range->get().getMin());
        }

        template<typename T>
        inline void INumericValueModel<T>::setMax()
        {
            setValue(_range->get().getMax());
        }

        template<typename T>
        inline std::shared_ptr<Core::Observer::IValueSubject<T> > INumericValueModel<T>::observeSmallIncrement() const
        {
            return _smallIncrement;
        }

        template<typename T>
        inline std::shared_ptr<Core::Observer::IValueSubject<T> > INumericValueModel<T>::observeLargeIncrement() const
        {
            return _largeIncrement;
        }

        template<typename T>
        inline void INumericValueModel<T>::incrementSmall()
        {
            setValue(_value->get() + _smallIncrement->get());
        }

        template<typename T>
        inline void INumericValueModel<T>::incrementLarge()
        {
            setValue(_value->get() + _largeIncrement->get());
        }

        template<typename T>
        inline void INumericValueModel<T>::decrementSmall()
        {
            setValue(_value->get() - _smallIncrement->get());
        }

        template<typename T>
        inline void INumericValueModel<T>::decrementLarge()
        {
            setValue(_value->get() - _largeIncrement->get());
        }

        template<typename T>
        inline void INumericValueModel<T>::setSmallIncrement(T value)
        {
            _smallIncrement->setIfChanged(value);
        }

        template<typename T>
        inline void INumericValueModel<T>::setLargeIncrement(T value)
        {
            _largeIncrement->setIfChanged(value);
        }

        template<typename T>
        inline std::shared_ptr<Core::Observer::IValueSubject<NumericValueOverflow> > INumericValueModel<T>::observeOverflow() const
        {
            return _overflow;
        }

        template<typename T>
        inline void INumericValueModel<T>::setOverflow(NumericValueOverflow value)
        {
            _overflow->setIfChanged(value);
        }

    } // namespace Math
} // namespace djv
