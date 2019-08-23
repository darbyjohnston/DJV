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
        namespace Range
        {
            template<>
            inline void Range<int64_t>::zero()
            {
                min = max = 0;
            }

        } // namespace Range
    
        namespace Frame
        {            
            inline Sequence::Sequence()
            {}

            inline Sequence::Sequence(const Range & range, size_t pad) :
                pad(pad)
            {
                ranges.push_back(range);
            }

            inline Sequence::Sequence(const std::vector<Range> & ranges, size_t pad) :
                ranges(ranges),
                pad(pad)
            {}

            inline bool Sequence::isValid() const
            {
                return ranges.size() > 0;
            }

            inline size_t Sequence::getSize() const
            {
                size_t out = 0;
                for (const auto& i : ranges)
                {
                    if (i.min < i.max)
                    {
                        out += i.max - i.min + 1;
                    }
                    else
                    {
                        out += i.min - i.max + 1;
                    }
                }
                return out;
            }

            inline Number Sequence::getFrame(size_t value) const
            {
                Number out = invalid;
                for (const auto& j : ranges)
                {
                    const size_t size = j.max - j.min + 1;
                    if (value < size)
                    {
                        out = j.min + value;
                        break;
                    }
                    value -= size;
                }
                return out;
            }

            inline bool Sequence::operator == (const Sequence & value) const
            {
                return ranges == value.ranges && pad == value.pad;
            }

            inline bool Sequence::operator != (const Sequence & value) const
            {
                return !(*this == value);
            }

            inline bool isValid(const Range & value)
            {
                return value != invalidRange;
            }

            inline std::vector<Number> toFrames(const Range & value)
            {
                std::vector<Number> out;
                for (auto i = value.min; i <= value.max; ++i)
                {
                    out.push_back(i);
                }
                return out;
            }

            inline std::vector<Number> toFrames(const Sequence & value)
            {
                std::vector<Number> out;
                for (const auto & range : value.ranges)
                {
                    for (const auto & i : toFrames(range))
                    {
                        out.push_back(i);
                    }
                }
                return out;
            }

            inline std::string toString(Number frame, size_t pad)
            {
                std::string out;

                const bool negative = frame < 0;
                const uint64_t abs = negative ? -frame : frame;

                char c[String::cStringLength] = "";
                const size_t length = String::intToString(abs, c);

                if (negative)
                {
                    out.push_back('-');
                }
                if (pad > length)
                {
                    for (size_t i = 0; i < pad - length; ++i)
                    {
                        out.push_back('0');
                    }
                }
                for (size_t i = 0; i < length; ++i)
                {
                    out.push_back(c[i]);
                }
                return out;
            }

            inline std::string toString(const Range & value, size_t pad)
            {
                std::vector<std::string> list;
                if (value.min != value.max)
                {
                    list.push_back(toString(value.min, pad));
                    list.push_back(toString(value.max, pad));
                }
                else
                {
                    list.push_back(toString(value.min, pad));
                }
                return String::join(list, '-');
            }
            
            inline void fromString(const std::string & value, Range & out, size_t & pad)
            {
                // Split the string into the minimum and maximum values (e.g., "1-10").
                const auto & pieces = String::split(value, '-');

                // Convert the minimum value.
                if (pieces.size() > 0 && pieces[0].size() > 0)
                {
                    out.min = std::stoi(pieces[0]);

                    // Check for zero padding.
                    if (pieces[0].size() >= 2 && '0' == pieces[0][0])
                    {
                        pad = pieces[0].size();
                    }
                }

                // Convert the maximum value.
                if (pieces.size() > 1 && pieces[1].size() > 0)
                {
                    out.max = std::stoi(pieces[1]);

                    // Check for zero padding.
                    if (pieces[1].size() >= 2 && '0' == pieces[1][0])
                    {
                        pad = std::max(pad, pieces[1].size());
                    }
                }
                else
                {
                    out.max = out.min;
                }
            }

            inline std::string toString(const Sequence & value)
            {
                std::vector<std::string> list;
                for (const auto & range : value.ranges)
                {
                    list.push_back(toString(range, value.pad));
                }
                return String::join(list, ',');
            }

            inline void fromString(const std::string & value, Sequence & out)
            {
                const auto & pieces = String::split(value, ',');
                for (const auto & piece : pieces)
                {
                    Range range;
                    size_t pad = 0;
                    fromString(piece, range, pad);
                    out.ranges.push_back(range);
                    out.pad = std::max(out.pad, pad);
                }
            }

        } // namespace Frame
    } // namespace Core
} // namespace djv
