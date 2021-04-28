// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/Range.h>

#include <djvCore/RapidJSON.h>

#include <vector>

namespace djv
{
    namespace Math
    {
        //! Frame numbers.
        namespace Frame
        {
            //! This type represents a frame number.
            typedef int64_t Number;

            //! This type represents a frame index.
            typedef int64_t Index;

            //! Invalid frame number.
            const int64_t invalid = std::numeric_limits<int64_t>::min();

            //! Invalid frame index.
            const int64_t invalidIndex = std::numeric_limits<int64_t>::min();

            //! Frame number range.
            typedef Math::Range<Number> Range;
            
            //! Sequence of frame numbers.
            //!
            //! A sequence is composed of multiple frame number ranges (e.g., 1-10,20-30).
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

            //! \name Utility
            ///@{

            bool isValid(const Range&);

            ///@}

            //! \name Conversion
            ///@{

            std::vector<Number> toFrames(const Range&);
            std::vector<Number> toFrames(const Sequence&);

            Sequence fromFrames(const std::vector<Number>&);

            std::string toString(Number, size_t pad = 0);
            std::string toString(const Range&, size_t pad = 0);
            std::string toString(const Sequence&);

            //! Throws:
            //! - std::exception
            void fromString(const std::string&, Range&, size_t& pad);

            //! Throws:
            //! - std::exception
            void fromString(const std::string&, Sequence&);

            ///@}

        } // namespace Frame
    } // namespace Math

    std::ostream& operator << (std::ostream&, const Math::Frame::Sequence&);

    //! Throws:
    //! - std::exception
    std::istream& operator >> (std::istream&, Math::Frame::Sequence&);

    rapidjson::Value toJSON(const Math::Frame::Sequence&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Math::Frame::Sequence&);
    
} // namespace djv

#include <djvMath/FrameNumberInline.h>
