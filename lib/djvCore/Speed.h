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

#pragma once

#include <djvCore/Enum.h>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            //! This class provides speed information.
            class Speed
            {
            public:
                enum class FPS
                {
                    _1,
                    _3,
                    _6,
                    _12,
                    _15,
                    _16,
                    _18,
                    _23_976,
                    _24,
                    _25,
                    _29_97,
                    _30,
                    _50,
                    _59_94,
                    _60,
                    _120,

                    Count
                };
                DJV_ENUM_HELPERS(FPS);

                Speed();
                Speed(int scale, int duration = 1);
                Speed(FPS);

                inline int getScale() const;
                inline int getDuration() const;
                inline bool isValid() const;

                static float speedToFloat(const Speed &);
                static Speed floatToSpeed(float);

                static FPS getDefaultSpeed();
                static FPS getGlobalSpeed();
                static void setGlobalSpeed(FPS);

                bool operator == (const Speed &) const;
                bool operator != (const Speed &) const;

            private:
                void _set(FPS);

                int _scale = 0;
                int _duration = 0;
            };

        } // namespace Time
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::Time::Speed::FPS);

    std::ostream & operator << (std::ostream &, const Core::Time::Speed &);
    std::istream & operator >> (std::istream &, Core::Time::Speed &);

} // namespace djv

#include <djvCore/SpeedInline.h>
