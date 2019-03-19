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

#include <djvCore/Math.h>

namespace djv
{
    namespace Core
    {
        template<typename T>
        inline void INumericValueModel<T>::_init()
        {
            _range          = ValueSubject<Range::tRange<T> >::create();
            _value          = ValueSubject<T>::create();
            _smallIncrement = ValueSubject<T>::create();
            _largeIncrement = ValueSubject<T>::create();
            _overflow       = ValueSubject<NumericValueOverflow>::create(NumericValueOverflow::Clamp);
        }

        template<typename T>
        inline INumericValueModel<T>::INumericValueModel()
        {}

        template<typename T>
        inline INumericValueModel<T>::~INumericValueModel()
        {}

        template<typename T>
        inline std::shared_ptr<IValueSubject<Range::tRange<T> > > INumericValueModel<T>::observeRange() const
        {
            return _range;
        }

        template<typename T>
        inline void INumericValueModel<T>::setRange(const Range::tRange<T> & value)
        {
            if (_range->setIfChanged(value))
            {
                setValue(_value->get());
            }
        }

        template<typename T>
        inline std::shared_ptr<IValueSubject<T> > INumericValueModel<T>::observeValue() const
        {
            return _value;
        }

        template<typename T>
        inline void INumericValueModel<T>::setValue(T value)
        {
            const auto & range = _range->get();
            switch (_overflow->get())
            {
            case NumericValueOverflow::Clamp:
                value = Math::clamp(value, range.min, range.max);
                break;
            case NumericValueOverflow::Wrap:
                if (range.min != range.max)
                {
                    while (value > range.max)
                    {
                        value -= range.max;
                    }
                    while (value < range.min)
                    {
                        value += range.min;
                    }
                }
                break;
            default: break;
            }
            _value->setIfChanged(value);
        }

        template<typename T>
        inline std::shared_ptr<IValueSubject<T> > INumericValueModel<T>::observeSmallIncrement() const
        {
            return _smallIncrement;
        }

        template<typename T>
        inline std::shared_ptr<IValueSubject<T> > INumericValueModel<T>::observeLargeIncrement() const
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
        inline std::shared_ptr<IValueSubject<NumericValueOverflow> > INumericValueModel<T>::observeOverflow() const
        {
            return _overflow;
        }

        template<typename T>
        inline void INumericValueModel<T>::setOverflow(NumericValueOverflow value)
        {
            _overflow->setIfChanged(value);
        }

    } // namespace Core
} // namespace djv
