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

//! \file djvImageApplicationTest.cpp

#include <djvImageApplicationTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvErrorUtil.h>
#include <djvImageApplication.h>
#include <djvOpenGlImage.h>

void djvImageApplicationTest::run(int & argc, char ** argv)
{
    DJV_DEBUG("djvImageApplicationTest::run");
    
    try
    {
        djvImageApplication app("test", argc, argv);

        DJV_ASSERT("test" == app.name());
        DJV_ASSERT(app.context());
            
        QStringList args;
        
        args += "arg";
        
        app.commandLine(args);
        
        DJV_ASSERT(1 == args.count());

        args += "-render_filter";
        args += "Box";
        args += "Triangle";
        
        app.commandLine(args);

        DJV_ASSERT(1 == args.count());
        DJV_ASSERT(djvOpenGlImageFilter(
            djvOpenGlImageFilter::BOX, djvOpenGlImageFilter::TRIANGLE) ==
            djvOpenGlImageFilter::filter());
        
        args += "-render_filter_high";
        
        app.commandLine(args);

        DJV_ASSERT(1 == args.count());
        DJV_ASSERT(djvOpenGlImageFilter::filterHighQuality() ==
            djvOpenGlImageFilter::filter());
        
        args += "-render_filter";

        try
        {    
            app.commandLine(args);
            
            DJV_ASSERT(0);
        }
        catch (...)
        {}
            
        args += "-help";
        
        app.commandLine(args);
        app.run();
        
        DJV_ASSERT(1 == args.count());
        DJV_ASSERT(djvApplicationEnum::EXIT_HELP == app.exitValue());
        
        args += "-info";
        
        app.commandLine(args);
        app.run();
        
        DJV_ASSERT(1 == args.count());
        DJV_ASSERT(djvApplicationEnum::EXIT_INFO == app.exitValue());
    }
    catch (const djvError & error)
    {
        djvErrorUtil::print(error);
        
        DJV_ASSERT(0);
    }
}

