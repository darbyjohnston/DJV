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

#include <djvViewApp/Enum.h>

namespace djv
{
    namespace ViewApp
    {
        float getImageRotate(ImageRotate value)
        {
            const float values[] =
            {
                0.F,
                90.F,
                180.F,
                270.F
            };
            return values[static_cast<size_t>(value)];
        }

        float getImageAspectRatio(ImageAspectRatio value)
        {
            const float values[] =
            {
                1.F,
                1.F,
                1.78F,
                1.85F,
                2.35F
            };
            return values[static_cast<size_t>(value)];
        }

    } // namespace ViewApp

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        ImageViewLock,
        DJV_TEXT("None"),
        DJV_TEXT("Full"),
        DJV_TEXT("Frame"),
        DJV_TEXT("Center"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        ImageRotate,
        DJV_TEXT("0"),
        DJV_TEXT("90"),
        DJV_TEXT("180"),
        DJV_TEXT("270"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        ImageAspectRatio,
        DJV_TEXT("Native"),
        DJV_TEXT("Default"),
        DJV_TEXT("16:9"),
        DJV_TEXT("1.85"),
        DJV_TEXT("2.35"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        Playback,
        DJV_TEXT("Stop"),
        DJV_TEXT("Forward"),
        DJV_TEXT("Reverse"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        PlaybackMode,
        DJV_TEXT("Once"),
        DJV_TEXT("Loop"),
        DJV_TEXT("PingPong"));

    picojson::value toJSON(ViewApp::ImageViewLock value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    picojson::value toJSON(ViewApp::ImageRotate value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    picojson::value toJSON(ViewApp::ImageAspectRatio value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    void fromJSON(const picojson::value& value, ViewApp::ImageViewLock& out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

    void fromJSON(const picojson::value& value, ViewApp::ImageRotate& out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

    void fromJSON(const picojson::value& value, ViewApp::ImageAspectRatio& out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

} // namespace djv

