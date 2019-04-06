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

#include <djvCore/Time.h>

#include <djvCore/String.h>

extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/rational.h>

} // extern "C"

#include <iomanip>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            Frame::Index timestampToFrame(Time::Timestamp value, const Time::Speed & speed)
            {
                AVRational r;
                r.num = speed.getDuration();
                r.den = speed.getScale();
                return av_rescale_q(value, av_get_time_base_q(), r);
            }

            Time::Timestamp frameToTimestamp(Frame::Index value, const Time::Speed & speed)
            {
                AVRational r;
                r.num = speed.getDuration();
                r.den = speed.getScale();
                return av_rescale_q(value, r, av_get_time_base_q());
            }

            double timestampToSeconds(Timestamp value)
            {
                return value / static_cast<double>(AV_TIME_BASE);
            }

            Timestamp secondsToTimestamp(double value)
            {
                return static_cast<Timestamp>(value * static_cast<double>(AV_TIME_BASE));
            }

            std::string getLabel(double value)
            {
                int    hours   = 0;
                int    minutes = 0;
                double seconds = 0.0;
                secondsToTime(value, hours, minutes, seconds);
                std::stringstream s;
                s << std::setfill('0') << std::setw(2) << hours;
                s << std::setw(0) << ":";
                s << std::setfill('0') << std::setw(2) << minutes;
                s << std::setw(0) << ":";
                s << std::setfill('0') << std::setw(2) << static_cast<int>(seconds);
                return s.str();
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
                    throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
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

                std::stringstream s;
                s << std::setfill('0') << std::setw(2) << hour;
                s << std::setw(0) << ":";
                s << std::setfill('0') << std::setw(2) << minute;
                s << std::setw(0) << ":";
                s << std::setfill('0') << std::setw(2) << second;
                s << std::setw(0) << ":";
                s << std::setfill('0') << std::setw(2) << frame;
                return s.str();
            }

            void stringToTimecode(const std::string & in, uint32_t & out, bool * ok)
            {
                if (ok)
                {
                    *ok = true;
                }

                int hour   = 0;
                int minute = 0;
                int second = 0;
                int frame  = 0;

                const auto pieces = String::split(in, ':');
                if (pieces.size() != 4 && ok)
                {
                    *ok = false;
                }

                int i = 0;
                switch (pieces.size())
                {
                case 4: hour   = std::stoi(pieces[i]); ++i;
                case 3: minute = std::stoi(pieces[i]); ++i;
                case 2: second = std::stoi(pieces[i]); ++i;
                case 1: frame  = std::stoi(pieces[i]); ++i;
                }

                out = timeToTimecode(hour, minute, second, frame);
            }

        } // namespace Time
    } // namespace Core
} // namespace djv
