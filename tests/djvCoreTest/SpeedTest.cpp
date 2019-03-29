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

#include <djvCoreTest/SpeedTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Math.h>
#include <djvCore/Speed.h>

#include <QStringList>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void SpeedTest::run(int &, char **)
        {
            DJV_DEBUG("SpeedTest::run");
            ctors();
            members();
            convert();
            operators();
        }

        void SpeedTest::ctors()
        {
            DJV_DEBUG("SpeedTest::ctors");
            {
                const Speed speed;
                const Speed speedDefault(Speed::speed());
                DJV_ASSERT(speedDefault.scale() == speed.scale());
                DJV_ASSERT(speedDefault.duration() == speed.duration());
            }
            {
                const Speed speed(24000, 1001);
                DJV_ASSERT(24000 == speed.scale());
                DJV_ASSERT(1001 == speed.duration());
            }
            {
                const Speed speed(Speed::FPS_23_976);
                DJV_ASSERT(24000 == speed.scale());
                DJV_ASSERT(1001 == speed.duration());
            }
        }

        void SpeedTest::members()
        {
            DJV_DEBUG("SpeedTest::members");
            {
                Speed::setSpeed(Speed::FPS_24);
                const Speed speed = Speed::speed();
                Speed::setSpeed(Speed::FPS_23_976);
                Speed::setSpeed(Speed::speedDefault());
                DJV_ASSERT(Speed::speed() == speed);
            }
            {
                Speed speed;
                speed.set(Speed::FPS_23_976);
                DJV_ASSERT(24000 == speed.scale());
                DJV_ASSERT(1001 == speed.duration());
            }
            {
                DJV_ASSERT(Speed().isValid());
                DJV_ASSERT(!Speed(0, 0).isValid());
            }
        }

        void SpeedTest::convert()
        {
            DJV_DEBUG("SpeedTest::convert");
            const struct Data
            {
                Speed speed;
                float value;
            }
            data[] =
            {
                { Speed(Speed::FPS_23_976),  23.976f },
                { Speed(Speed::FPS_24),      24.f    },
                { Speed(Speed::FPS_29_97),   29.97f  },
                { Speed(240, 1),            240.f    }
            };
            const int dataCount = sizeof(data) / sizeof(data[0]);
            for (int i = 0; i < dataCount; ++i)
            {
                DJV_DEBUG_PRINT("speed = " << data[i].speed);
                const float value = Speed::speedToFloat(data[i].speed);
                DJV_DEBUG_PRINT("value = " << value);
                DJV_ASSERT(Math::fuzzyCompare(value, data[i].value, .0001f));
                const Speed speed = Speed::floatToSpeed(value);
                DJV_ASSERT(data[i].speed == speed);
            }
        }

        void SpeedTest::operators()
        {
            DJV_DEBUG("SpeedTest::operators");
            {
                const Speed a(Speed::FPS_23_976), b(Speed::FPS_23_976);
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != Speed());
            }
            {
                Speed speed;
                QStringList tmp;
                tmp << speed;
                tmp >> speed;
                DJV_ASSERT(speed == Speed());
            }
            {
                DJV_DEBUG_PRINT(Speed());
            }
        }

    } // namespace CoreTest
} // namespace djv
