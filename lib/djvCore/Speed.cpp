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
//   and/or other materials provided with the distribution.3
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

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Math.h>

namespace djv
{
    namespace Core
    {
        const QStringList & Speed::fpsLabels()
        {
            static const QStringList data = QStringList() <<
                "1" <<
                "3" <<
                "6" <<
                "12" <<
                "15" <<
                "16" <<
                "18" <<
                "23.976" <<
                "24" <<
                "25" <<
                "29.97" <<
                "30" <<
                "50" <<
                "59.94" <<
                "60" <<
                "120";
            DJV_ASSERT(data.count() == FPS_COUNT);
            return data;
        }

        namespace
        {
            Speed::FPS _speed = Speed::speedDefault();

        } // namespace

        Speed::Speed()
        {
            set(speed());
        }

        Speed::Speed(int scale, int duration) :
            _scale(scale),
            _duration(duration)
        {}

        Speed::Speed(FPS in)
        {
            set(in);
        }

        int Speed::scale() const
        {
            return _scale;
        }

        int Speed::duration() const
        {
            return _duration;
        }

        void Speed::set(FPS fps)
        {
            static const int scale[] =
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
            DJV_ASSERT(sizeof(scale) / sizeof(scale[0]) == FPS_COUNT);
            static const int duration[] =
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
            DJV_ASSERT(sizeof(duration) / sizeof(duration[0]) == FPS_COUNT);
            _scale = scale[fps];
            _duration = duration[fps];
        }

        bool Speed::isValid() const
        {
            return _scale != 0 && _duration != 0;
        }

        float Speed::speedToFloat(const Speed & speed)
        {
            return speed._scale / static_cast<float>(speed._duration);
        }

        Speed Speed::floatToSpeed(float value)
        {
            //! \todo Implement a proper floating-point to rational number conversion.
            //! Check-out: OpenEXR\IlmImf\ImfRational.h
            for (int i = 0; i < FPS_COUNT; ++i)
            {
                const FPS fps = static_cast<FPS>(i);
                if (Math::abs(value - speedToFloat(fps)) < 0.001f)
                {
                    return fps;
                }
            }
            return Speed(Math::round(value));
        }

        Speed::FPS Speed::speedDefault()
        {
            return FPS_24;
        }

        Speed::FPS Speed::speed()
        {
            return _speed;
        }

        void Speed::setSpeed(FPS fps)
        {
            _speed = fps;
        }

    } // namespace Core

    bool operator == (const Core::Speed & a, const Core::Speed & b)
    {
        return a.scale() == b.scale() && a.duration() == b.duration();
    }

    bool operator != (const Core::Speed & a, const Core::Speed & b)
    {
        return !(a == b);
    }

    _DJV_STRING_OPERATOR_LABEL(Core::Speed::FPS, Core::Speed::fpsLabels());

    QStringList & operator >> (QStringList & string, Core::Speed & out)
    {
        int scale = 0;
        int duration = 0;
        string >> scale;
        string >> duration;
        out = Core::Speed(scale, duration);
        return string;
    }

    QStringList & operator << (QStringList & out, const Core::Speed & in)
    {
        return out << in.scale() << in.duration();
    }

    Core::Debug & operator << (Core::Debug & debug, const Core::Speed & in)
    {
        return debug << Core::Speed::speedToFloat(in);
    }

} // namespace djv
