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

namespace djv
{
    namespace Core
    {
	    namespace Time
	    {
		    inline void secondsToTime(double in, int & hour, int & minute, double & seconds)
		    {
			    hour = static_cast<int>(in) / (60 * 60);
			    in -= hour * 60 * 60;
			    minute = static_cast<int>(in) / 60;
			    in -= minute * 60;
			    seconds = in;
		    }

            inline void timecodeToTime(
                uint32_t in,
                int &    hour,
                int &    minute,
                int &    seconds,
                int &    frame)
            {
                hour    = (in >> 28 & 0x0f) * 10 + (in >> 24 & 0x0f);
                minute  = (in >> 20 & 0x0f) * 10 + (in >> 16 & 0x0f);
                seconds = (in >> 12 & 0x0f) * 10 + (in >> 8 & 0x0f);
                frame   = (in >> 4 & 0x0f) * 10 + (in >> 0 & 0x0f);
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

            inline int64_t timecodeToFrame(uint32_t in, const Speed & speed)
            {
                if (!speed.isValid())
                    return 0;
                int hour    = 0;
                int minute  = 0;
                int seconds = 0;
                int frame   = 0;
                timecodeToTime(in, hour, minute, seconds, frame);
                return static_cast<int64_t>((hour * 60 * 60 + minute * 60 + seconds) * Speed::speedToFloat(speed)) + frame;
            }

            inline uint32_t frameToTimecode(int64_t frame, const Speed& speed)
            {
                if (!speed.isValid())
                    return 0;
                const float speedF = Speed::speedToFloat(speed);
                const int hour = static_cast<int>(frame / (speedF * 60 * 60));
                frame -= static_cast<int>(hour * speedF * 60 * 60);
                const int minute = static_cast<int>(frame / (speedF * 60));
                frame -= static_cast<int>(minute * speedF * 60);
                const int second = static_cast<int>(frame / speedF);
                frame -= static_cast<int>(second * speedF);
                return timeToTimecode(hour, minute, second, static_cast<int>(frame));
            }

	    } // namespace Time
	} // namespace Core
} // namespace djv
