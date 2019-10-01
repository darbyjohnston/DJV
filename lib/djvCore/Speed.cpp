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

#include <djvCore/Speed.h>

#include <djvCore/String.h>

#include <algorithm>
#include <cmath>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            Math::Rational toRational(FPS fps)
            {
                const int scale[] =
                {
                    1,
                    3,
                    6,
                    12,
                    15,
                    16,
                    18,
                    24000,
                    24,
                    25,
                    30000,
                    30,
                    50,
                    60000,
                    60,
                    120
                };
                DJV_ASSERT(static_cast<size_t>(FPS::Count) == sizeof(scale) / sizeof(scale[0]));
                const int duration[] =
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1001,
                    1,
                    1,
                    1001,
                    1,
                    1,
                    1001,
                    1,
                    1
                };
                DJV_ASSERT(static_cast<size_t>(FPS::Count) == sizeof(duration) / sizeof(duration[0]));
                return Math::Rational(scale[static_cast<size_t>(fps)], duration[static_cast<size_t>(fps)]);
            }

            namespace
            {
                FPS defaultSpeed = FPS::_24;

            } // namespace

            FPS fromRational(const Math::Rational& value)
            {
                //! \todo Implement a proper floating-point to rational number conversion.
                //! Check-out: OpenEXR\IlmImf\ImfRational.h
                for (size_t i = 0; i < static_cast<size_t>(FPS::Count); ++i)
                {
                    const FPS fps = static_cast<FPS>(i);
                    if (fabs(value.toFloat() - toRational(fps).toFloat()) < .000001F)
                    {
                        return fps;
                    }
                }
                return defaultSpeed;
            }

            FPS getDefaultSpeed()
            {
                return defaultSpeed;
            }

            void setDefaultSpeed(FPS value)
            {
                defaultSpeed = value;
            }

            Speed::Speed() :
                Math::Rational(toRational(defaultSpeed))
            {}

            Speed::Speed(FPS fps) :
                Math::Rational(toRational(fps))
            {}

            Speed::Speed(int scale, int duration) :
                Math::Rational(scale, duration)
            {}

            Speed::Speed(const Math::Rational& value) :
                Math::Rational(value)
            {}

        } // namespace Time
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Time,
        FPS,
        "1",
        "3",
        "6",
        "12",
        "15",
        "16",
        "18",
        "23.976",
        "24",
        "25",
        "29.97",
        "30",
        "50",
        "59.94",
        "60",
        "120");

} // namespace djv
