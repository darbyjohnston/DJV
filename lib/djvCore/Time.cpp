// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Time.h>

#include <djvCore/Speed.h>
#include <djvCore/StringFunc.h>

#include <array>
#include <climits>
#include <iomanip>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            Math::Rational getTimebaseRational()
            {
                return Math::Rational(1, timebase);
            }

            int64_t scale(int64_t value, const Math::Rational& br, const Math::Rational& cr) noexcept
            {
                int64_t out = 0;
                const int64_t b = br.getNum() * static_cast<int64_t>(cr.getDen());
                const int64_t c = cr.getNum() * static_cast<int64_t>(br.getDen());
                //! \bug The FFmpeg documentation for av_rescale_q() says that this can overflow?
                if (b <= INT_MAX && c <= INT_MAX)
                {
                    const int64_t r = c / 2;
                    out = (value * b + r) / c;
                }
                else
                {
                    DJV_ASSERT(0);
                }
                return out;
            }
            
            void secondsToTime(double in, int& hour, int& minute, double& seconds)
            {
                hour     = static_cast<int>(in / 3600);
                in      -= static_cast<int64_t>(hour) * static_cast<int64_t>(3600);
                minute   = in / 60;
                in      -= static_cast<int64_t>(minute) * static_cast<int64_t>(60);
                seconds  = in;
            }

            std::string getLabel(double value)
            {
                int    hours   = 0;
                int    minutes = 0;
                double seconds = 0.0;
                secondsToTime(value, hours, minutes, seconds);
                std::stringstream ss;
                ss << std::setfill('0') << std::setw(2) << hours;
                ss << std::setw(0) << ":";
                ss << std::setfill('0') << std::setw(2) << minutes;
                ss << std::setw(0) << ":";
                ss << std::setfill('0') << std::setw(2) << static_cast<int>(seconds);
                return ss.str();
            }

            std::string getLabel(time_t value)
            {
                std::tm tm;
                localtime(&value, &tm);
                char buffer[32];
                std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", &tm);
                return buffer;
            }

            void localtime(const time_t * t, tm * tm)
            {
#if defined(DJV_PLATFORM_WINDOWS)
                localtime_s(tm, t);
#else // DJV_PLATFORM_WINDOWS
                localtime_r(t, tm);
#endif // DJV_PLATFORM_WINDOWS
            }
            
            std::string keycodeToString(
                int id,
                int type,
                int prefix,
                int count,
                int offset)
            {
                std::vector<std::string> list;
                list.push_back(std::to_string(id));
                list.push_back(std::to_string(type));
                list.push_back(std::to_string(prefix));
                list.push_back(std::to_string(count));
                list.push_back(std::to_string(offset));
                return String::join(list, ':');
            }

            void stringToKeycode(
                const std::string& string,
                int&               id,
                int&               type,
                int&               prefix,
                int&               count,
                int&               offset)
            {
                const auto pieces = String::split(string, ':');
                if (pieces.size() != 5)
                {
                    //! \todo How can we translate this?
                    throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
                }
                id     = std::stoi(pieces[0]);
                type   = std::stoi(pieces[1]);
                prefix = std::stoi(pieces[2]);
                count  = std::stoi(pieces[3]);
                offset = std::stoi(pieces[4]);
            }

            inline void timecodeToTime(
                uint32_t in,
                int& hour,
                int& minute,
                int& seconds,
                int& frame)
            {
                hour    = (in >> 28 & 0x0f) * 10 + (in >> 24 & 0x0f);
                minute  = (in >> 20 & 0x0f) * 10 + (in >> 16 & 0x0f);
                seconds = (in >> 12 & 0x0f) * 10 + (in >>  8 & 0x0f);
                frame   = (in >>  4 & 0x0f) * 10 + (in >>  0 & 0x0f);
            }

            inline uint32_t timeToTimecode(
                int hour,
                int minute,
                int seconds,
                int frame)
            {
                return
                    (hour    / 10 & 0x0f) << 28 | (hour    % 10 & 0x0f) << 24 |
                    (minute  / 10 & 0x0f) << 20 | (minute  % 10 & 0x0f) << 16 |
                    (seconds / 10 & 0x0f) << 12 | (seconds % 10 & 0x0f) <<  8 |
                    (frame   / 10 & 0x0f) <<  4 | (frame   % 10 & 0x0f) <<  0;
            }

            inline Frame::Number timecodeToFrame(uint32_t in, const Math::Rational& r)
            {
                if (!r.isValid())
                    return 0;
                int hour    = 0;
                int minute  = 0;
                int seconds = 0;
                int frame   = 0;
                timecodeToTime(in, hour, minute, seconds, frame);
                return (
                    static_cast<int64_t>(hour) * static_cast<int64_t>(60) * static_cast<int64_t>(60) +
                    static_cast<int64_t>(minute) * static_cast<int64_t>(60) +
                    static_cast<int64_t>(seconds)
                    ) *
                    static_cast<double>(r.toFloat()) +
                    static_cast<int64_t>(frame);
            }

            inline uint32_t frameToTimecode(Frame::Number frame, const Math::Rational& r)
            {
                if (!r.isValid())
                    return 0;
                const float f = r.toFloat();
                const int hour = static_cast<int>(frame / (f * 60 * 60));
                frame -= static_cast<Frame::Number>(hour) * static_cast<double>(f) * static_cast<Frame::Number>(60) * static_cast<Frame::Number>(60);
                const int minute = static_cast<int>(frame / (f * 60));
                frame -= static_cast<Frame::Number>(minute) * static_cast<double>(f) * static_cast<Frame::Number>(60);
                const int second = static_cast<int>(frame / f);
                frame -= static_cast<Frame::Number>(second) * static_cast<double>(f);
                return timeToTimecode(hour, minute, second, static_cast<int>(frame));
            }

            std::string timecodeToString(uint32_t in)
            {
                int hour   = 0;
                int minute = 0;
                int second = 0;
                int frame  = 0;
                timecodeToTime(in, hour, minute, second, frame);

                /*std::stringstream ss;
                ss << std::setfill('0') << std::setw(2) << hour;
                ss << std::setw(0) << ":";
                ss << std::setfill('0') << std::setw(2) << minute;
                ss << std::setw(0) << ":";
                ss << std::setfill('0') << std::setw(2) << second;
                ss << std::setw(0) << ":";
                ss << std::setfill('0') << std::setw(2) << frame;
                return ss.str();*/

                std::string out = "00:00:00:00";
                out[ 0] = 48 +   hour / 10;
                out[ 1] = 48 +   hour % 10;
                out[ 3] = 48 + minute / 10;
                out[ 4] = 48 + minute % 10;
                out[ 6] = 48 + second / 10;
                out[ 7] = 48 + second % 10;
                out[ 9] = 48 +  frame / 10;
                out[10] = 48 +  frame % 10;
                return out;
            }

            void stringToTimecode(const std::string& in, uint32_t& out)
            {
                int hour   = 0;
                int minute = 0;
                int second = 0;
                int frame  = 0;
                const auto pieces = String::split(in, ':');
                size_t i = 0;
                switch (pieces.size())
                {
                case 4:
                    hour   = std::stoi(pieces[i]); ++i;
                    minute = std::stoi(pieces[i]); ++i;
                    second = std::stoi(pieces[i]); ++i;
                    frame  = std::stoi(pieces[i]);
                    break;
                case 3:
                    minute = std::stoi(pieces[i]); ++i;
                    second = std::stoi(pieces[i]); ++i;
                    frame  = std::stoi(pieces[i]);
                    break;
                case 2:
                    second = std::stoi(pieces[i]); ++i;
                    frame  = std::stoi(pieces[i]);
                    break;
                case 1:
                    frame = std::stoi(pieces[i]);
                    break;
                default:
                    //! \todo How can we translate this?
                    throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
                }
                out = timeToTimecode(hour, minute, second, frame);
            }

            std::string toString(Frame::Number value, const Math::Rational& speed, Units units)
            {
                std::string out;
                switch (units)
                {
                case Units::Timecode:
                {
                    const uint32_t timecode = frameToTimecode(value, speed);
                    out = timecodeToString(timecode);
                    break;
                }
                case Units::Frames:
                {
                    std::stringstream ss;
                    ss << value;
                    out = ss.str();
                    break;
                }
                default: break;
                }
                return out;
            }

            Frame::Number fromString(const std::string& value, const Math::Rational& speed, Units units)
            {
                Frame::Number out = Frame::invalid;
                switch (units)
                {
                case Units::Timecode:
                {
                    uint32_t timecode = 0;
                    Time::stringToTimecode(value, timecode);
                    out = Time::timecodeToFrame(timecode, speed);
                    break;
                }
                case Units::Frames:
                    out = std::stoi(value);
                    break;
                default: break;
                }
                return out;
            }

            DJV_ENUM_HELPERS_IMPLEMENTATION(Units);

        } // namespace Time
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Time,
        Units,
        DJV_TEXT("time_units_timecode"),
        DJV_TEXT("time_units_frames"));

    rapidjson::Value toJSON(Core::Time::Units value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, Core::Time::Units& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv
