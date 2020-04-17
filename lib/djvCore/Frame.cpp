// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
            
            bool Sequence::merge(const Range& value)
            {
                bool out = false;
                for (auto& i : ranges)
                {
                    if (i.intersects(value))
                    {
                        i.min = std::min(i.min, value.min);
                        i.max = std::max(i.max, value.max);
                        out = true;
                        break;
                    }
                    if (value.max == i.min - 1)
                    {
                        i.min = value.min;
                        out = true;
                        break;
                    }
                    if (value.min == i.max + 1)
                    {
                        i.max = value.max;
                        out = true;
                        break;
                    }
                }
                return out;
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
        s.exceptions(std::istream::failbit | std::istream::badbit);

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

