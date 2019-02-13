//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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
			inline void _init();
            inline INumericValueModel();

		public:
            inline virtual ~INumericValueModel() = 0;

            inline std::shared_ptr<IValueSubject<Range::tRange<T> > > observeRange() const;
            inline void setRange(const Range::tRange<T> &);

            inline std::shared_ptr<IValueSubject<T> > observeValue() const;
            inline void setValue(T);

            inline std::shared_ptr<IValueSubject<T> > observeIncrement() const;
            inline void incrementValue();
            inline void decrementValue();
            inline void setIncrement(T);

            inline std::shared_ptr<IValueSubject<NumericValueOverflow> > observeOverflow() const;
            inline void setOverflow(NumericValueOverflow);

		private:
            std::shared_ptr<ValueSubject<Range::tRange<T> > >    _range;
            std::shared_ptr<ValueSubject<T> >                    _value;
            std::shared_ptr<ValueSubject<T> >                    _increment;
            std::shared_ptr<ValueSubject<NumericValueOverflow> > _overflow;
		};

	} // namespace Core
} // namespace djv

#include <djvCore/INumericValueModelInline.h>
