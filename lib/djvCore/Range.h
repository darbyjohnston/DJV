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

#include <djvCore/String.h>

namespace djv
{
    namespace Core
    {
        //! This namespace provides number range functionality.
        namespace Range
        {
            //! This class provides a range of numbers.
            template<typename T>
            struct tRange
            {
                inline tRange();
                inline tRange(T minMax);
                inline tRange(T min, T max);
                virtual inline ~tRange();

                //! \name Range Components
                ///@{

                T min = static_cast<T>(0);
                T max = static_cast<T>(0);

                ///@}

                //! \name Range Utilities
                ///@{

                inline T getSize() const;
                inline void zero();

                inline bool contains(T) const;

                inline bool intersects(const tRange<T>&) const;

                inline void expand(T);
                inline void expand(const tRange<T>&);

                inline T getRandom() const;

                ///@}

                inline bool operator == (const tRange<T>&) const;
                inline bool operator != (const tRange<T>&) const;
                inline bool operator  < (const tRange<T>&) const;
            };

        } // namespace Range

        //! This typedef provides an integer range.
		typedef Range::tRange<int> IntRange;

		//! This typedef provides a floating point range.
		typedef Range::tRange<float> FloatRange;

    } // namespace Core

    template<typename T>
    inline std::ostream& operator << (std::ostream&, const Core::Range::tRange<T>&);
    template<typename T>
    inline std::istream& operator >> (std::istream&, Core::Range::tRange<T>&);

} // namespace djv

#include <djvCore/RangeInline.h>
