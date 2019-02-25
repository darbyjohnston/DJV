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

namespace djv
{
    namespace Core
    {
        //! This namespace provides frame number functionality.
        namespace Frame
        {
            //! This type represents a frame number.
            typedef int64_t Number;

            //! This type represents a frame index.
            typedef int64_t Index;

            //! This value represents an invalid frame.
            const int64_t Invalid = -1;

            //! This typedef provides a frame range.
            typedef Core::Range::tRange<int64_t> Range;

            //! This value represents an invalid range.
            const Range InvalidRange = Range(Invalid);

            //! This struct provides a sequence of frame numbers. A sequence is
            //! composed of multiple frame ranges (e.g., 1-10,20-30).
            struct Sequence
            {
                inline Sequence();
                inline Sequence(Number);
                inline Sequence(Number min, Number max);
                inline Sequence(const Range&, size_t pad = 0);
                inline Sequence(const std::vector<Range>&, size_t pad = 0);

                std::vector<Range> ranges;
                size_t pad = 0;

                bool isValid() const { return ranges.size() > 0; }

                //! \name Sequence Utilities
                ///@{
                
                //! Sort the sequence so that the numbers are in ascending order.
                void sort();
                
                ///@}

                inline bool operator == (const Sequence&) const;
                inline bool operator != (const Sequence&) const;
            };

            //! \name Frame Number Utilities
            ///@{

            inline bool isValid(const Range&);

            inline Number getFrame(const std::vector<Number>&, Index index);

            void sort(Range&);

            ///@}

            //! \name Frame Number Conversion
            ///@{

            inline std::vector<Number> toFrames(const Range&);
            inline std::vector<Number> toFrames(const Sequence&);
            Sequence fromFrames(const std::vector<Number>&);

            inline std::string toString(Number, size_t pad);
            inline std::string toString(const Range&, size_t pad);
            inline std::string toString(const Sequence&);

            //! Throws:
            //! - std::exception
            inline void fromString(const std::string&, Range&, size_t& pad);

            //! Throws:
            //! - std::exception
            inline void fromString(const std::string&, Sequence&);

            ///@}

        } // namespace Frame
    } // namespace Core

    std::ostream& operator << (std::ostream&, const Core::Frame::Sequence&);
    std::istream& operator >> (std::istream&, Core::Frame::Sequence&);

} // namespace djv

#include <djvCore/FrameInline.h>
