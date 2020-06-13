// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Range.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Core
    {
        //! This enumeration provides options for what happens when the
        //! value range is exceeded.
        enum class NumericValueOverflow
        {
            Clamp,
            Wrap
        };

        //! This class provides an interface for numeric value models.
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

            std::shared_ptr<IValueSubject<Range::Range<T> > > observeRange() const;
            void setRange(const Range::Range<T>&);

            ///@}

            //! \name Value
            ///@{

            std::shared_ptr<IValueSubject<T> > observeValue() const;
            std::shared_ptr<IValueSubject<bool> > observeIsMin() const;
            std::shared_ptr<IValueSubject<bool> > observeIsMax() const;
            void setValue(T);
            void setMin();
            void setMax();

            ///@}

            //! \name Increments
            ///@{

            std::shared_ptr<IValueSubject<T> > observeSmallIncrement() const;
            std::shared_ptr<IValueSubject<T> > observeLargeIncrement() const;
            void incrementSmall();
            void incrementLarge();
            void decrementSmall();
            void decrementLarge();
            void setSmallIncrement(T);
            void setLargeIncrement(T);

            ///@}

            //! \name Overflow
            ///@{

            std::shared_ptr<IValueSubject<NumericValueOverflow> > observeOverflow() const;
            void setOverflow(NumericValueOverflow);

            ///@}

        private:
            std::shared_ptr<ValueSubject<Range::Range<T> > >     _range;
            std::shared_ptr<ValueSubject<T> >                    _value;
            std::shared_ptr<ValueSubject<bool> >                 _isMin;
            std::shared_ptr<ValueSubject<bool> >                 _isMax;
            std::shared_ptr<ValueSubject<T> >                    _smallIncrement;
            std::shared_ptr<ValueSubject<T> >                    _largeIncrement;
            std::shared_ptr<ValueSubject<NumericValueOverflow> > _overflow;
        };

    } // namespace Core
} // namespace djv

#include <djvCore/INumericValueModelInline.h>
