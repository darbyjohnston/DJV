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

#include <djvAV/Timecode.h>

#include <djvCore/String.h>

#include <iomanip>
#include <sstream>

namespace djv
{
    namespace AV
    {
        namespace Timecode
        {
		    std::string toString(uint32_t in)
		    {
			    int hour   = 0;
			    int minute = 0;
			    int second = 0;
			    int frame  = 0;
			    toTime(in, hour, minute, second, frame);

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

		    void fromString(const std::string& in, uint32_t& out, bool* ok)
		    {
			    if (ok)
			    {
				    *ok = true;
			    }

			    int hour   = 0;
			    int minute = 0;
			    int second = 0;
			    int frame  = 0;

			    const auto pieces = Core::String::split(in, ':');
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

			    out = fromTime(hour, minute, second, frame);
		    }

	    } // namespace Timecode
	} // namespace AV
} // namespace djv
