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

#include <djvAVTest/AVContextTest.h>

#include <djvAV/AVContext.h>
#include <djvAV/OpenGLImage.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/ErrorUtil.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        void AVContextTest::run(int & argc, char ** argv)
        {
            DJV_DEBUG("AVContextTest::run");
            {
                AV::AVContext context(argc, argv);
                DJV_ASSERT(context.openGLContext());
            }
            try
            {
                AV::AVContext context(argc, argv);
                char * args[256] =
                {
                    "djvTest",
                    "arg"
                };
                int argsCount = 2;
                DJV_ASSERT(context.commandLine(argsCount, args));
            }
            catch (const Error & error)
            {
                ErrorUtil::print(error);
                DJV_ASSERT(0);
            }
            try
            {
                AV::AVContext context(argc, argv);
                char * args[256] =
                {
                    "djvTest",
                    "-render_filter", "Box", "Triangle"
                };
                int argsCount = 4;
                DJV_ASSERT(context.commandLine(argsCount, args));
                DJV_ASSERT(AV::OpenGLImageFilter(
                    AV::OpenGLImageFilter::BOX, AV::OpenGLImageFilter::TRIANGLE) ==
                    AV::OpenGLImageFilter::filter());
            }
            catch (const Error & error)
            {
                ErrorUtil::print(error);
                DJV_ASSERT(0);
            }
            try
            {
                AV::AVContext context(argc, argv);
                char * args[256] =
                {
                    "djvTest",
                    "-render_filter_high"
                };
                int argsCount = 2;
                DJV_ASSERT(context.commandLine(argsCount, args));
                DJV_ASSERT(AV::OpenGLImageFilter::filterHighQuality() == AV::OpenGLImageFilter::filter());
            }
            catch (const Error & error)
            {
                ErrorUtil::print(error);
                DJV_ASSERT(0);
            }
            try
            {
                AV::AVContext context(argc, argv);
                char * args[256] =
                {
                    "djvTest",
                    "-render_filter"
                };
                int argsCount = 2;
                DJV_ASSERT(!context.commandLine(argsCount, args));
            }
            catch (...)
            {
            }

            try
            {
                AV::AVContext context(argc, argv);
                char * args[256] =
                {
                    "djvTest",
                    "-help",
                };
                int argsCount = 2;
                DJV_ASSERT(!context.commandLine(argsCount, args));
            }
            catch (const Error & error)
            {
                ErrorUtil::print(error);
                DJV_ASSERT(0);
            }
            try
            {
                AV::AVContext context(argc, argv);
                char * args[256] =
                {
                    "djvTest",
                    "-info",
                };
                int argsCount = 2;
                DJV_ASSERT(!context.commandLine(argsCount, args));
            }
            catch (const Error & error)
            {
                ErrorUtil::print(error);
                DJV_ASSERT(0);
            }
        }

    } // namespace AVTest
} // namespace djv
