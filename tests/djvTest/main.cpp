//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvCoreTest/CacheTest.h>
#include <djvCoreTest/EnumTest.h>
#include <djvCoreTest/FileIOTest.h>
#include <djvCoreTest/FrameTest.h>
#include <djvCoreTest/MathTest.h>
#include <djvCoreTest/MemoryTest.h>
#include <djvCoreTest/ObjectTest.h>
#include <djvCoreTest/PathTest.h>
#include <djvCoreTest/SpeedTest.h>
#include <djvCoreTest/StringTest.h>
#include <djvCoreTest/TimeTest.h>

#if not defined(DJV_TINY_BUILD)
#include <djvAVTest/AudioTest.h>
#include <djvAVTest/ColorTest.h>
#include <djvAVTest/PixelTest.h>
#endif // DJV_TINY_BUILD

#include <djvCore/Context.h>
#include <djvCore/Error.h>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        std::vector<std::string> args;
        auto context = Core::Context::create(args);

        (new CoreTest::CacheTest(context))->run(args);
        (new CoreTest::EnumTest(context))->run(args);
        (new CoreTest::FrameTest(context))->run(args);
        (new CoreTest::FileIOTest(context))->run(args);
        (new CoreTest::MathTest(context))->run(args);
        (new CoreTest::MemoryTest(context))->run(args);
        (new CoreTest::ObjectTest(context))->run(args);
        (new CoreTest::PathTest(context))->run(args);
        (new CoreTest::SpeedTest(context))->run(args);
        (new CoreTest::StringTest(context))->run(args);
        (new CoreTest::TimeTest(context))->run(args);

#if not defined(DJV_TINY_BUILD)
        (new AVTest::AudioTest(context))->run(args);
        (new AVTest::ColorTest(context))->run(args);
        (new AVTest::PixelTest(context))->run(args);
#endif // DJV_TINY_BUILD
    }
    catch (const std::exception & error)
    {
        std::cout << Core::Error::format(error) << std::endl;
    }
    return r;
}
