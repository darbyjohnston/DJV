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

            std::shared_ptr<IValueSubject<Range::Range<T> > > observeRange() const;
            void setRange(const Range::Range<T> &);

            std::shared_ptr<IValueSubject<T> > observeValue() const;
            std::shared_ptr<IValueSubject<bool> > observeIsMin() const;
            std::shared_ptr<IValueSubject<bool> > observeIsMax() const;
            void setValue(T);
            void setMin();
            void setMax();

            std::shared_ptr<IValueSubject<T> > observeSmallIncrement() const;
            std::shared_ptr<IValueSubject<T> > observeLargeIncrement() const;
            void incrementSmall();
            void incrementLarge();
            void decrementSmall();
            void decrementLarge();
            void setSmallIncrement(T);
            void setLargeIncrement(T);

            std::shared_ptr<IValueSubject<NumericValueOverflow> > observeOverflow() const;
            void setOverflow(NumericValueOverflow);

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
