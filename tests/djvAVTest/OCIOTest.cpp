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

#include <djvAVTest/OCIOTest.h>

#include <djvAV/OCIO.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        OCIOTest::OCIOTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::OCIOTest", context)
        {}
        
        void OCIOTest::run(const std::vector<std::string>& args)
        {
            _convert();
            _view();
            _display();
            _operators();
        }

        void OCIOTest::_convert()
        {
            {
                const OCIO::Convert convert;
                DJV_ASSERT(convert.input.empty());
                DJV_ASSERT(convert.output.empty());
                DJV_ASSERT(!convert.isValid());
            }
            
            {
                const std::string input = "input";
                const std::string output = "output";
                const OCIO::Convert convert(input, output);
                DJV_ASSERT(input == convert.input);
                DJV_ASSERT(output == convert.output);
                DJV_ASSERT(convert.isValid());
            }
        }
        
        void OCIOTest::_view()
        {
            {
                const OCIO::View view;
                DJV_ASSERT(view.name.empty());
                DJV_ASSERT(view.colorSpace.empty());
                DJV_ASSERT(view.looks.empty());
            }
        }
        
        void OCIOTest::_display()
        {
            {
                const OCIO::Display display;
                DJV_ASSERT(display.name.empty());
                DJV_ASSERT(display.defaultView.empty());
                DJV_ASSERT(display.views.empty());
            }
        }
        
        void OCIOTest::_operators()
        {
            {
                const OCIO::Convert convert("input", "output");
                DJV_ASSERT(convert == convert);
                DJV_ASSERT(OCIO::Convert() < convert);
            }
            
            {
                OCIO::View view;
                view.name = "name";
                view.colorSpace = "colorSpace";
                view.looks = "looks";
                DJV_ASSERT(view == view);
            }
            
            {
                OCIO::Display display;
                display.name = "name";
                display.defaultView = "defaultView";
                display.views.push_back(OCIO::View());
                DJV_ASSERT(display == display);
            }
        }

    } // namespace AVTest
} // namespace djv

