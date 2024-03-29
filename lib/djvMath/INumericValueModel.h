// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/Range.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Math
    {
        //! Numeric value overflow options.
        enum class NumericValueOverflow
        {
            Clamp,
            Wrap
        };

        //! Base class for numeric value models.
        template<typename T>
        class INumericValueModel
        {
            DJV_NON_COPYABLE(INumericValueModel);

        protected:
            void _init();
            INumericValueModel();

        public:
            virtual ~INumericValueModel() = 0;

            //! \name Range
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<Math::Range<T> > > observeRange() const;
            
            void setRange(const Math::Range<T>&);

            ///@}

            //! \name Value
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<T> > observeValue() const;
            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeIsMin() const;
            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeIsMax() const;
            
            void setValue(T);
            void setMin();
            void setMax();

            ///@}

            //! \name Increment
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<T> > observeSmallIncrement() const;
            std::shared_ptr<Core::Observer::IValueSubject<T> > observeLargeIncrement() const;
            
            void incrementSmall();
            void incrementLarge();
            void decrementSmall();
            void decrementLarge();
            void setSmallIncrement(T);
            void setLargeIncrement(T);

            ///@}

            //! \name Overflow
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<NumericValueOverflow> > observeOverflow() const;
            
            void setOverflow(NumericValueOverflow);

            ///@}

        private:
            std::shared_ptr<Core::Observer::ValueSubject<Math::Range<T> > >      _range;
            std::shared_ptr<Core::Observer::ValueSubject<T> >                    _value;
            std::shared_ptr<Core::Observer::ValueSubject<bool> >                 _isMin;
            std::shared_ptr<Core::Observer::ValueSubject<bool> >                 _isMax;
            std::shared_ptr<Core::Observer::ValueSubject<T> >                    _smallIncrement;
            std::shared_ptr<Core::Observer::ValueSubject<T> >                    _largeIncrement;
            std::shared_ptr<Core::Observer::ValueSubject<NumericValueOverflow> > _overflow;
        };

    } // namespace Math
} // namespace djv

#include <djvMath/INumericValueModelInline.h>
