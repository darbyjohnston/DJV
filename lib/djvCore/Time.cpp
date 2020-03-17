//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvCore/Time.h>

#include <djvCore/Speed.h>
#include <djvCore/String.h>

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

            int64_t scale(int64_t value, const Math::Rational& br, const Math::Rational& cr)
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
                const std::string & string,
                int &               id,
                int &               type,
                int &               prefix,
                int &               count,
                int &               offset)
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

            void stringToTimecode(const std::string & in, uint32_t & out)
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

            std::string toString(Frame::Number value, const Speed& speed, Units units)
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

            Frame::Number fromString(const std::string& value, const Time::Speed& speed, Units units)
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

        } // namespace Time
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Time,
        Units,
        DJV_TEXT("time_units_timecode"),
        DJV_TEXT("time_units_frames"));

    picojson::value toJSON(Core::Time::Units value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    void fromJSON(const picojson::value& value, Core::Time::Units& out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv
