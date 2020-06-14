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

        } // namespace Frame
    } // namespace Core

    std::ostream& operator << (std::ostream& s, const Core::Frame::Sequence& value)
    {
        std::vector<std::string> pieces;
        for (const auto& range : value.getRanges())
        {
            pieces.push_back(Core::Frame::toString(range, value.getPad()));
        }
        s << Core::String::join(pieces, ',');
        return s;
    }

    std::istream& operator >> (std::istream& s, Core::Frame::Sequence& out)
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
                Core::Frame::Range range;
                Core::Frame::fromString(piece, range, pad);
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

    rapidjson::Value toJSON(const Core::Frame::Sequence& value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, Core::Frame::Sequence& out)
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
                out = Core::Frame::Sequence();
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

