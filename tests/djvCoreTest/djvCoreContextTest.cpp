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

#include <djvCoreContextTest.h>

#include <djvCore/Assert.h>
#include <djvCore/CoreContext.h>
#include <djvCore/Debug.h>
#include <djvCore/ErrorUtil.h>
#include <djvCore/Math.h>
#include <djvCore/Speed.h>
#include <djvCore/Time.h>

void djvCoreContextTest::run(int & argc, char ** argv)
{
    DJV_DEBUG("djvCoreContextTest::run");
    {
        djvCoreContext context;
        DJV_DEBUG_PRINT("doc = " << context.doc());
        context.printError(djvError("prefix", "string"));
        context.printSeparator();
        context.print("message 2");
    }

    try
    {
        djvCoreContext context;
        char * args [256] =
        {
            "djvTest",
            "arg"
        };
        int argsCount = 2;
        DJV_ASSERT(context.commandLine(argsCount, args));
    }
    catch (const djvError & error)
    {
        djvErrorUtil::print(error);
        DJV_ASSERT(0);
    }

    try
    {
        djvCoreContext context;
        char * args [256] =
        {
            "djvTest",
            "-time_units",    "timecode",
            "-default_speed", "12"
        };
        int argsCount = 5;
        DJV_ASSERT(context.commandLine(argsCount, args));
        DJV_ASSERT(djvTime::UNITS_TIMECODE == djvTime::units());
        DJV_ASSERT(djvSpeed(12) == djvSpeed::speed());
    }
    catch (const djvError & error)
    {
        djvErrorUtil::print(error);
        DJV_ASSERT(0);
    }
    try
    {
        djvCoreContext context;
        char * args [256] =
        {
            "djvTest",
            "-time_units",
        };
        int argsCount = 2;
        DJV_ASSERT(! context.commandLine(argsCount, args));
    }
    catch (...)
    {}
    try
    {
        djvCoreContext context;
        char * args [256] =
        {
            "djvTest",
            "-help",
        };
        int argsCount = 2;
        DJV_ASSERT(! context.commandLine(argsCount, args));
    }
    catch (const djvError & error)
    {
        djvErrorUtil::print(error);
        DJV_ASSERT(0);
    }
    try
    {
        djvCoreContext context;
        char * args [256] =
        {
            "djvTest",
            "-info",
        };
        int argsCount = 2;
        DJV_ASSERT(! context.commandLine(argsCount, args));
    }
    catch (const djvError & error)
    {
        djvErrorUtil::print(error);
        DJV_ASSERT(0);
    }
    try
    {
        djvCoreContext context;
        char * args [256] =
        {
            "djvTest",
            "-about",
        };
        int argsCount = 2;
        DJV_ASSERT(! context.commandLine(argsCount, args));
    }
    catch (const djvError & error)
    {
        djvErrorUtil::print(error);
        DJV_ASSERT(0);
    }
}

