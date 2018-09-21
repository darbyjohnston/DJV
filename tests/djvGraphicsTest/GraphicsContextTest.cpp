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

#include <djvGraphicsTest/GraphicsContextTest.h>

#include <djvGraphics/GraphicsContext.h>
#include <djvGraphics/OpenGLImage.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/ErrorUtil.h>

using namespace djv::Core;
using namespace djv::Graphics;

namespace djv
{
    namespace GraphicsTest
    {
        void GraphicsContextTest::run(int & argc, char ** argv)
        {
            DJV_DEBUG("GraphicsContextTest::run");
            {
                Graphics::GraphicsContext context;
                DJV_ASSERT(context.openGLContext());
            }
            try
            {
                Graphics::GraphicsContext context;
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
                Graphics::GraphicsContext context;
                char * args[256] =
                {
                    "djvTest",
                    "-render_filter", "Box", "Triangle"
                };
                int argsCount = 4;
                DJV_ASSERT(context.commandLine(argsCount, args));
                DJV_ASSERT(Graphics::OpenGLImageFilter(
                    Graphics::OpenGLImageFilter::BOX, Graphics::OpenGLImageFilter::TRIANGLE) ==
                    Graphics::OpenGLImageFilter::filter());
            }
            catch (const Error & error)
            {
                ErrorUtil::print(error);
                DJV_ASSERT(0);
            }
            try
            {
                Graphics::GraphicsContext context;
                char * args[256] =
                {
                    "djvTest",
                    "-render_filter_high"
                };
                int argsCount = 2;
                DJV_ASSERT(context.commandLine(argsCount, args));
                DJV_ASSERT(Graphics::OpenGLImageFilter::filterHighQuality() == Graphics::OpenGLImageFilter::filter());
            }
            catch (const Error & error)
            {
                ErrorUtil::print(error);
                DJV_ASSERT(0);
            }
            try
            {
                Graphics::GraphicsContext context;
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
                Graphics::GraphicsContext context;
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
                Graphics::GraphicsContext context;
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

    } // namespace GraphicsTest
} // namespace djv
