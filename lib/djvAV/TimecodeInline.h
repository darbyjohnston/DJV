//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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
    namespace AV
    {
	    namespace Timecode
	    {
		    inline void toTime(
			    uint32_t in,
			    int&     hour,
			    int&     minute,
			    int&     second,
			    int&     frame)
		    {
			    hour   = (in >> 28 & 0x0f) * 10 + (in >> 24 & 0x0f);
			    minute = (in >> 20 & 0x0f) * 10 + (in >> 16 & 0x0f);
			    second = (in >> 12 & 0x0f) * 10 + (in >>  8 & 0x0f);
			    frame  = (in >>  4 & 0x0f) * 10 + (in >>  0 & 0x0f);
		    }

		    inline uint32_t fromTime(
			    int hour,
			    int minute,
			    int second,
			    int frame)
		    {
			    return
				    (hour   / 10 & 0x0f) << 28 | (  hour % 10 & 0x0f) << 24 |
				    (minute / 10 & 0x0f) << 20 | (minute % 10 & 0x0f) << 16 |
				    (second / 10 & 0x0f) << 12 | (second % 10 & 0x0f) <<  8 |
				    (frame  / 10 & 0x0f) <<  4 | ( frame % 10 & 0x0f) <<  0;
		    }

		    inline int64_t toFrame(uint32_t in, const Speed & speed)
		    {
			    if (!speed.isValid())
				    return 0;

			    int hour   = 0;
			    int minute = 0;
			    int second = 0;
			    int frame  = 0;
			    toTime(in, hour, minute, second, frame);

			    return static_cast<int64_t>((hour * 60 * 60 + minute * 60 + second) * Speed::speedToFloat(speed)) + frame;
		    }

		    inline uint32_t fromFrame(int64_t frame, const Speed& speed)
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

			    return fromTime(hour, minute, second, static_cast<int>(frame));
		    }

	    } // namespace Timecode
	} // namespace AV
} // namespace djv
