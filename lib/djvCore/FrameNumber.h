// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Range.h>

#include <vector>

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

            //! This value represents an invalid frame number.
            const int64_t invalid = std::numeric_limits<int64_t>::min();

            //! This value represents an invalid frame index.
            const int64_t invalidIndex = std::numeric_limits<int64_t>::min();

            //! This typedef provides a frame number range.
            typedef Math::Range<Number> Range;
            
            //! This class provides a sequence of frame numbers. A sequence is
            //! composed of multiple frame number ranges (e.g., 1-10,20-30).
            class Sequence
            {
            public:
                Sequence();
                explicit Sequence(Number);
                Sequence(Number min, Number max, size_t pad = 0);
                explicit Sequence(const Range& range, size_t pad = 0);
                explicit Sequence(const std::vector<Range>& ranges, size_t pad = 0);

                //! \name Ranges
                ///@{

                const std::vector<Range>& getRanges() const noexcept;
                
                void add(const Range&);

                bool isValid() const noexcept;

                ///@}

                //! \name Padding
                ///@{

                size_t getPad() const noexcept;
                
                void setPad(size_t);

                ///@}

                //! \name Frames
                ///@{

                bool contains(Index) const noexcept;
                
                size_t getFrameCount() const noexcept;
                Number getFrame(Index) const noexcept;
                
                Index getIndex(Number) const noexcept;
                Index getLastIndex() const noexcept;

                ///@}

                bool operator == (const Sequence&) const;
                bool operator != (const Sequence&) const;

            private:
                std::vector<Range>  _ranges;
                size_t              _pad    = 0;
            };

        } // namespace Frame
    } // namespace Core
} // namespace djv

