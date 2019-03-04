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

#include <djvCoreTest/EnumTest.h>
#include <djvCoreTest/MathTest.h>
#include <djvCoreTest/MemoryTest.h>
#include <djvCoreTest/PathTest.h>
#include <djvCoreTest/StringTest.h>

#include <djvAVTest/AudioTest.h>
#include <djvAVTest/ColorTest.h>
#include <djvAVTest/PixelTest.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        auto context = Core::Context::create(argc, argv);

        //(new CoreTest::EnumTest(context.get()))->run(argc, argv);
        //(new CoreTest::MathTest(context.get()))->run(argc, argv);
        //(new CoreTest::MemoryTest(context.get()))->run(argc, argv);
        (new CoreTest::PathTest(context.get()))->run(argc, argv);
        //(new CoreTest::StringTest(context.get()))->run(argc, argv);

        //(new AVTest::AudioTest(context.get()))->run(argc, argv);
        //(new AVTest::ColorTest(context.get()))->run(argc, argv);
        //(new AVTest::PixelTest(context.get()))->run(argc, argv);
    }
    catch (const std::exception & error)
    {
        std::cout << Core::Error::format(error) << std::endl;
    }
    return r;
}
