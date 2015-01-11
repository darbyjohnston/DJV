//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//3
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

//! \file djvSpeed.cpp

#include <djvSpeed.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvMath.h>

//------------------------------------------------------------------------------
// djvSpeed
//------------------------------------------------------------------------------

const QStringList & djvSpeed::fpsLabels()
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

djvSpeed::FPS _speed = djvSpeed::speedDefault();

} // namespace

djvSpeed::djvSpeed()
{
    set(speed());
}

djvSpeed::djvSpeed(int scale, int duration) :
    _scale   (scale),
    _duration(duration)
{}

djvSpeed::djvSpeed(FPS in)
{
    set(in);
}
    
int djvSpeed::scale() const
{
    return _scale;
}

int djvSpeed::duration() const
{
    return _duration;
}

void djvSpeed::set(FPS fps)
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
    
    _scale    = scale   [fps];
    _duration = duration[fps];
}

bool djvSpeed::isValid() const
{
    return _scale != 0 && _duration != 0;
}

double djvSpeed::speedToFloat(const djvSpeed & speed)
{
    return speed._scale / static_cast<double>(speed._duration);
}

djvSpeed djvSpeed::floatToSpeed(double value)
{
    //! \todo Implement a proper floating-point to rational number conversion.
    
    for (int i = 0; i < FPS_COUNT; ++i)
    {
        const FPS fps = static_cast<FPS>(i);
        
        if (djvMath::abs(value - speedToFloat(fps)) < 0.001)
        {
            return fps;
        }
    }
    
    return djvSpeed(djvMath::round(value));
}

djvSpeed::FPS djvSpeed::speedDefault()
{
    return FPS_24;
}

djvSpeed::FPS djvSpeed::speed()
{
    return _speed;
}

void djvSpeed::setSpeed(FPS fps)
{
    _speed = fps;
}

//------------------------------------------------------------------------------

bool operator == (const djvSpeed & a, const djvSpeed & b)
{
    return a.scale() == b.scale() && a.duration() == b.duration();
}

bool operator != (const djvSpeed & a, const djvSpeed & b)
{
    return ! (a == b);
}

_DJV_STRING_OPERATOR_LABEL(djvSpeed::FPS, djvSpeed::fpsLabels())

QStringList & operator >> (QStringList & string, djvSpeed & out) throw (QString)
{
    int scale    = 0;
    int duration = 0;
    
    string >> scale;
    string >> duration;
    
    out = djvSpeed(scale, duration);

    return string;
}

QStringList & operator << (QStringList & out, const djvSpeed & in)
{
    return out << in.scale() << in.duration();
}

djvDebug & operator << (djvDebug & debug, const djvSpeed & in)
{
    return debug << djvSpeed::speedToFloat(in);
}

