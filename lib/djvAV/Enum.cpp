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

#include <djvAV/Enum.h>

#include <algorithm>

using namespace djv::Core;

namespace djv
{
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV,
        Side,
        DJV_TEXT("None"),
        DJV_TEXT("menu_view_left"),
        DJV_TEXT("direction_top"),
        DJV_TEXT("menu_view_right"),
        DJV_TEXT("direction_bottom"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV,
        TimeUnits,
        DJV_TEXT("unit_time_timecode"),
        DJV_TEXT("unit_time_frames"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV,
        AlphaBlend,
        DJV_TEXT("None"),
        DJV_TEXT("alpha_straight"),
        DJV_TEXT("alpha_premultiplied"));

    picojson::value toJSON(AV::AlphaBlend value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    void fromJSON(const picojson::value& value, AV::AlphaBlend& out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

