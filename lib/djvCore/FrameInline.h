// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
                _min = _max = 0;
            }

        } // namespace Range
    
        namespace Frame
        {            
            inline Sequence::Sequence()
            {}
       
            inline Sequence::Sequence(Number number)
            {
                _ranges.push_back(Range(number));
            }
       
            inline Sequence::Sequence(Number min, Number max, size_t pad) :
                _pad(pad)
            {
                _ranges.push_back(Range(min, max));
            }

            inline Sequence::Sequence(const Range & range, size_t pad) :
                _pad(pad)
            {
                _ranges.push_back(range);
            }

            inline Sequence::Sequence(const std::vector<Range> & ranges, size_t pad) :
                _pad(pad)
            {
                for (const auto& i : ranges)
                {
                    add(i);
                }
            }

            inline const std::vector<Range>& Sequence::getRanges() const
            {
                return _ranges;
            }

            inline bool Sequence::isValid() const
            {
                return _ranges.size() > 0;
            }

            inline size_t Sequence::getPad() const
            {
                return _pad;
            }

            inline void Sequence::setPad(size_t value)
            {
                _pad = value;
            }

            inline bool Sequence::contains(Index value) const
            {
                bool out = false;
                for (const auto& i : _ranges)
                {
                    if (i.contains(value))
                    {
                        out = true;
                        break;
                    }
                }
                return out;
            }

            inline size_t Sequence::getFrameCount() const
            {
                size_t out = 0;
                for (const auto& i : _ranges)
                {
                    out += i.getMax() - i.getMin() + 1;
                }
                return out;
            }

            inline Number Sequence::getFrame(Index value) const
            {
                Number out = invalid;
                for (const auto& j : _ranges)
                {
                    const size_t size = j.getMax() - j.getMin() + 1;
                    if (value < size)
                    {
                        out = j.getMin() + value;
                        break;
                    }
                    value -= size;
                }
                return out;
            }

            inline Index Sequence::getIndex(Number value) const
            {
                Index out = invalidIndex;
                Index tmp = 0;
                for (const auto& j : _ranges)
                {
                    if (j.contains(value))
                    {
                        out = tmp + value - j.getMin();
                        break;
                    }
                    tmp += j.getMax() - j.getMin() + 1;
                }
                return out;
            }

            inline Index Sequence::getLastIndex() const
            {
                Index out = 0;
                for (const auto& i : _ranges)
                {
                    if (i.getMin() < i.getMax())
                    {
                        out += i.getMax() - i.getMin() + 1;
                    }
                    else
                    {
                        out += i.getMin() - i.getMax() + 1;
                    }
                }
                return out > 0 ? (out - 1) : 0;
            }

            inline bool Sequence::operator == (const Sequence & value) const
            {
                return _ranges == value._ranges && _pad == value._pad;
            }

            inline bool Sequence::operator != (const Sequence & value) const
            {
                return !(*this == value);
            }

            inline std::vector<Number> toFrames(const Range & value)
            {
                std::vector<Number> out;
                for (auto i = value.getMin(); i <= value.getMax(); ++i)
                {
                    out.push_back(i);
                }
                return out;
            }

            inline std::vector<Number> toFrames(const Sequence & value)
            {
                std::vector<Number> out;
                for (const auto & range : value.getRanges())
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
                if (value.getMin() != value.getMax())
                {
                    list.push_back(toString(value.getMin(), pad));
                    list.push_back(toString(value.getMax(), pad));
                }
                else
                {
                    list.push_back(toString(value.getMin(), pad));
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
                    out = Range(std::stoi(pieces[0]));

                    // Check for zero padding.
                    if (pieces[0].size() >= 2 && '0' == pieces[0][0])
                    {
                        pad = pieces[0].size();
                    }
                }

                // Convert the maximum value.
                if (pieces.size() > 1 && pieces[1].size() > 0)
                {
                    out = Range(out.getMin(), std::stoi(pieces[1]));

                    // Check for zero padding.
                    if (pieces[1].size() >= 2 && '0' == pieces[1][0])
                    {
                        pad = std::max(pad, pieces[1].size());
                    }
                }
                else
                {
                    out = Range(out.getMin());
                }
            }

            inline std::string toString(const Sequence & value)
            {
                std::vector<std::string> list;
                for (const auto & range : value.getRanges())
                {
                    list.push_back(toString(range, value.getPad()));
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
                    out.add(range);
                    out.setPad(std::max(out.getPad(), pad));
                }
            }

        } // namespace Frame
    } // namespace Core
} // namespace djv
