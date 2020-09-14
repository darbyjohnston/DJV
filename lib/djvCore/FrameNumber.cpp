// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/FrameNumber.h>

#include <djvCore/Math.h>
#include <djvCore/StringFunc.h>

#include <algorithm>
#include <sstream>

namespace djv
{
    namespace Core
    {
        namespace Frame
        {
            Sequence::Sequence()
            {}
       
            Sequence::Sequence(Number number)
            {
                _ranges.push_back(Range(number));
            }
       
            Sequence::Sequence(Number min, Number max, size_t pad) :
                _pad(pad)
            {
                _ranges.push_back(Range(min, max));
            }

            Sequence::Sequence(const Range& range, size_t pad) :
                _pad(pad)
            {
                _ranges.push_back(range);
            }

            Sequence::Sequence(const std::vector<Range>& ranges, size_t pad) :
                _pad(pad)
            {
                for (const auto& i : ranges)
                {
                    add(i);
                }
            }

            const std::vector<Range>& Sequence::getRanges() const noexcept
            {
                return _ranges;
            }
            
            void Sequence::add(const Range& value)
            {
                Range newRange(value);
                auto i = _ranges.begin();
                while (i != _ranges.end())
                {
                    if (newRange.intersects(*i))
                    {
                        newRange.expand(*i);
                        i = _ranges.erase(i);
                    }
                    else if (newRange.getMin() == i->getMax() + 1)
                    {
                        newRange = Range(i->getMin(), newRange.getMax());
                        i = _ranges.erase(i);
                    }
                    else if (newRange.getMax() == i->getMin() - 1)
                    {
                        newRange = Range(newRange.getMin(), i->getMax());
                        i = _ranges.erase(i);
                    }
                    else
                    {
                        ++i;
                    }
                }
                i = _ranges.begin();
                for (; i != _ranges.end() && *i < newRange; ++i)
                    ;
                _ranges.insert(i, newRange);
            }

            bool Sequence::isValid() const noexcept
            {
                return _ranges.size() > 0;
            }

            size_t Sequence::getPad() const noexcept
            {
                return _pad;
            }

            void Sequence::setPad(size_t value)
            {
                _pad = value;
            }

            bool Sequence::contains(Index value) const noexcept
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

            size_t Sequence::getFrameCount() const noexcept
            {
                size_t out = 0;
                for (const auto& i : _ranges)
                {
                    out += i.getMax() - i.getMin() + 1;
                }
                return out;
            }

            Number Sequence::getFrame(Index value) const noexcept
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

            Index Sequence::getIndex(Number value) const noexcept
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

            Index Sequence::getLastIndex() const noexcept
            {
                Index tmp = 0;
                for (const auto& i : _ranges)
                {
                    tmp += i.getMax() - i.getMin() + 1;
                }
                return _ranges.size() ? (tmp - 1) : invalidIndex;
            }

            bool Sequence::operator == (const Sequence& value) const
            {
                return _ranges == value._ranges && _pad == value._pad;
            }

            bool Sequence::operator != (const Sequence& value) const
            {
                return !(*this == value);
            }
            
        } // namespace Frame
    } // namespace Core
} // namespace djv

