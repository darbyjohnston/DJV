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
//
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

//! \file djvSpeedTest.cpp

#include <djvSpeedTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvMath.h>
#include <djvSpeed.h>

#include <QStringList>

void djvSpeedTest::run(int &, char **)
{
    DJV_DEBUG("djvSpeedTest::run");
    
    ctors();
    members();
    convert();
    operators();
}

void djvSpeedTest::ctors()
{
    DJV_DEBUG("djvSpeedTest::ctors");
    
    {
        const djvSpeed speed;
        
        const djvSpeed speedDefault(djvSpeed::speed());
        
        DJV_ASSERT(speedDefault.scale()    == speed.scale());
        DJV_ASSERT(speedDefault.duration() == speed.duration());
    }
    
    {
        const djvSpeed speed(24000, 1001);
        
        DJV_ASSERT(24000 == speed.scale());
        DJV_ASSERT(1001  == speed.duration());
    }
    
    {
        const djvSpeed speed(djvSpeed::FPS_23_976);
        
        DJV_ASSERT(24000 == speed.scale());
        DJV_ASSERT(1001  == speed.duration());
    }
}

void djvSpeedTest::members()
{
    DJV_DEBUG("djvSpeedTest::members");
    
    {
        djvSpeed::setSpeed(djvSpeed::FPS_24);
        
        const djvSpeed speed = djvSpeed::speed();
        
        djvSpeed::setSpeed(djvSpeed::FPS_23_976);
        djvSpeed::setSpeed(djvSpeed::speedDefault());
        
        DJV_ASSERT(djvSpeed::speed() == speed);
    }
    
    {
        djvSpeed speed;
        
        speed.set(djvSpeed::FPS_23_976);
        
        DJV_ASSERT(24000 == speed.scale());
        DJV_ASSERT(1001  == speed.duration());
    }
    
    {
        DJV_ASSERT(djvSpeed().isValid());
        
        DJV_ASSERT(! djvSpeed(0, 0).isValid());
    }
}

void djvSpeedTest::convert()
{
    DJV_DEBUG("djvSpeedTest::convert");
    
    const struct Data
    {
        djvSpeed speed;
        double   value;
    }
        data [] =
    {
        { djvSpeed(djvSpeed::FPS_23_976),  23.976 },
        { djvSpeed(djvSpeed::FPS_24),      24.0   },
        { djvSpeed(djvSpeed::FPS_29_97),   29.97  },
        { djvSpeed(240, 1),               240.0   }
    };
    
    const int dataCount = sizeof(data) / sizeof(data[0]);
    
    for (int i = 0; i < dataCount; ++i)
    {
        DJV_DEBUG_PRINT("speed = " << data[i].speed);
        
        const double value = djvSpeed::speedToFloat(data[i].speed);
        
        DJV_DEBUG_PRINT("value = " << value);
        
        DJV_ASSERT(djvMath::fuzzyCompare(value, data[i].value, 0.0001));
        
        const djvSpeed speed = djvSpeed::floatToSpeed(value);
        
        DJV_ASSERT(data[i].speed == speed);
    }
}

void djvSpeedTest::operators()
{
    DJV_DEBUG("djvSpeedTest::operators");
    
    {
        const djvSpeed a(djvSpeed::FPS_23_976), b(djvSpeed::FPS_23_976);
        
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvSpeed());
    }
    
    {
        djvSpeed speed;
        
        QStringList tmp;
        
        tmp << speed;
        
        tmp >> speed;
        
        DJV_ASSERT(speed == djvSpeed());
    }
    
    {
        DJV_DEBUG_PRINT(djvSpeed());
    }
}

