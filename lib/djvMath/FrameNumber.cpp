// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvMath/FrameNumber.h>
#include <djvMath/Math.h>

#include <djvCore/String.h>

#include <algorithm>
#include <sstream>

namespace djv
{
    namespace Math
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

            std::vector<Number> toFrames(const Range& value)
            {
                std::vector<Number> out;
                for (auto i = value.getMin(); i <= value.getMax(); ++i)
                {
                    out.push_back(i);
                }
                return out;
            }

            std::vector<Number> toFrames(const Sequence& value)
            {
                std::vector<Number> out;
                for (const auto& range : value.getRanges())
                {
                    for (const auto& i : toFrames(range))
                    {
                        out.push_back(i);
                    }
                }
                return out;
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
                            out.add(Range(rangeStart, prevFrame));
                            rangeStart = frames[i];
                        }
                    }
                    if (size > 1)
                    {
                        out.add(Range(rangeStart, prevFrame));
                    }
                    else
                    {
                        out.add(Range(rangeStart));
                    }
                }
                return out;
            }

            std::string toString(Number frame, size_t pad)
            {
                std::string out;

                const bool negative = frame < 0;
                const uint64_t abs = negative ? -frame : frame;

                char c[Core::String::cStringLength] = "";
                const size_t length = Core::String::intToString(abs, c);

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

            std::string toString(const Range& value, size_t pad)
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
                return Core::String::join(list, '-');
            }
            
            void fromString(const std::string& value, Range& out, size_t& pad)
            {
                // Split the string into the minimum and maximum values (e.g., "1-10").
                const auto& pieces = Core::String::split(value, '-');

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

            std::string toString(const Sequence& value)
            {
                std::vector<std::string> list;
                for (const auto& range : value.getRanges())
                {
                    list.push_back(toString(range, value.getPad()));
                }
                return Core::String::join(list, ',');
            }

            void fromString(const std::string& value, Sequence& out)
            {
                const auto& pieces = Core::String::split(value, ',');
                for (const auto& piece : pieces)
                {
                    Range range;
                    size_t pad = 0;
                    fromString(piece, range, pad);
                    out.add(range);
                    out.setPad(std::max(out.getPad(), pad));
                }
            }
            
        } // namespace Frame
    } // namespace Math

    std::ostream& operator << (std::ostream& s, const Math::Frame::Sequence& value)
    {
        std::vector<std::string> pieces;
        for (const auto& range : value.getRanges())
        {
            pieces.push_back(Math::Frame::toString(range, value.getPad()));
        }
        s << Core::String::join(pieces, ',');
        return s;
    }

    std::istream& operator >> (std::istream& s, Math::Frame::Sequence& out)
    {
        try
        {
            s.exceptions(std::istream::failbit | std::istream::badbit);

            std::string tmp;
            s >> tmp;

            // Split the string into ranges (e.g., 1-10,20-30).
            const auto pieces = Core::String::split(tmp, ',');

            // Convert the ranges.
            size_t pad = 0;
            for (const auto& piece : pieces)
            {
                Math::Frame::Range range;
                Math::Frame::fromString(piece, range, pad);
                out.add(range);
                out.setPad(std::max(pad, out.getPad()));
            }
        }
        catch (const std::exception&)
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
        return s;
    }

    rapidjson::Value toJSON(const Math::Frame::Sequence& value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, Math::Frame::Sequence& out)
    {
        if (value.IsString())
        {
            if (const char* s = value.GetString())
            {
                std::stringstream ss(s);
                ss >> out;
            }
            else
            {
                out = Math::Frame::Sequence();
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

