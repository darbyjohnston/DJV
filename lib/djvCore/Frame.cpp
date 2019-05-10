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

#include <djvCore/Frame.h>

#include <djvCore/Math.h>

#include <algorithm>
#include <sstream>

namespace djv
{
    namespace Core
    {
        namespace Frame
        {
            void Sequence::sort()
            {
                for (auto & range : ranges)
                {
                    Frame::sort(range);
                }

                std::sort(ranges.begin(), ranges.end());

                if (ranges.size())
                {
                    std::vector<Range> tmp;
                    tmp.push_back(ranges[0]);
                    for (size_t i = 1; i < ranges.size(); ++i)
                    {
                        const size_t tmpSize = tmp.size();
                        size_t j = 0;
                        for (; j < tmpSize; ++j)
                        {
                            if (ranges[i].min == tmp[j].max + 1)
                            {
                                tmp[j].max = ranges[i].max;
                                break;
                            }
                            else if (ranges[i].intersects(tmp[j]))
                            {
                                tmp[j].expand(ranges[i]);
                                break;
                            }
                        }
                        if (tmpSize == j)
                        {
                            tmp.push_back(ranges[i]);
                        }
                    }
                    ranges = tmp;
                }
            }

            void sort(Range & out)
            {
                const auto _min = std::min(out.min, out.max);
                const auto _max = std::max(out.min, out.max);
                out.min = _min;
                out.max = _max;
            }

            Sequence fromFrames(const std::vector<Number> & frames)
            {
                Sequence out;
                const size_t size = frames.size();
                if (size)
                {
                    Frame::Number rangeStart = frames[0];
                    Frame::Number prevFrame = frames[0];
                    size_t i = 1;
                    for (; i < size; prevFrame = frames[i], ++i)
                    {
                        if (frames[i] != prevFrame + 1)
                        {
                            out.ranges.push_back(Range(rangeStart, prevFrame));
                            rangeStart = frames[i];
                        }
                    }
                    if (size > 1)
                    {
                        out.ranges.push_back(Range(rangeStart, prevFrame));
                    }
                    else
                    {
                        out.ranges.push_back(Range(rangeStart));
                    }
                }
                return out;
            }

        } // namespace Frame
    } // namespace Core

    std::ostream & operator << (std::ostream & s, const Core::Frame::Sequence & value)
    {
        std::vector<std::string> pieces;
        for (const auto & range : value.ranges)
        {
            pieces.push_back(Core::Frame::toString(range, value.pad));
        }
        s << Core::String::join(pieces, ',');
        return s;
    }

    std::istream & operator >> (std::istream & s, Core::Frame::Sequence & out)
    {
        std::string tmp;
        s >> tmp;

        // Split the string into ranges (e.g., 1-10,20-30).
        const auto pieces = Core::String::split(tmp, ',');

        // Convert the ranges.
        size_t pad = 0;
        for (const auto & piece : pieces)
        {
            Core::Frame::Range range;
            Core::Frame::fromString(piece, range, pad);
            out.ranges.push_back(range);
            out.pad = std::max(pad, out.pad);
        }
        return s;
    }

} // namespace djv

