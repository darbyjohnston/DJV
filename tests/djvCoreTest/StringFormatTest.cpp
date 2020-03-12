//------------------------------------------------------------------------------
// Copyright (c) 2020 Darby Johnston
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

#include <djvCoreTest/StringFormatTest.h>

#include <djvCore/StringFormat.h>

#include <iostream>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        StringFormatTest::StringFormatTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::StringFormatTest", context)
        {}
        
        void StringFormatTest::run()
        {
            {
                std::stringstream ss;
                std::string text;
                const std::string result = String::Format(text);
                ss << text << ": " << result;
                _print(ss.str());
                DJV_ASSERT(result == std::string());
            }
            {
                std::stringstream ss;
                std::string text = "testing";
                const std::string result = String::Format(text);
                ss << text << ": " << result;
                _print(ss.str());
                DJV_ASSERT(result == "testing");
            }
            {
                std::stringstream ss;
                std::string text = "testing {0}";
                const std::string result = String::Format(text).arg("testing");
                ss << text << ": " << result;
                _print(ss.str());
                DJV_ASSERT(result == "testing testing");
            }
            {
                std::stringstream ss;
                std::string text = "testing {0} {1} {2}";
                const std::string result = String::Format(text).arg("one").arg("two").arg("three");
                ss << text << ": " << result;
                _print(ss.str());
                DJV_ASSERT(result == "testing one two three");
            }
            {
                std::stringstream ss;
                std::string text = "testing {2} {1} {0}";
                const std::string result = String::Format(text).arg("one").arg("two").arg("three");
                ss << text << ": " << result;
                _print(ss.str());
                DJV_ASSERT(result == "testing three two one");
            }
            {
                std::stringstream ss;
                std::string text = "testing";
                String::Format f = String::Format(text).arg("one");
                ss << text << ": " << f.getError();
                _print(ss.str());
                DJV_ASSERT(f.hasError());
            }
            {
                std::stringstream ss;
                std::string text = "testing { 0 }";
                String::Format f = String::Format(text).arg("one");
                ss << text << ": " << f.getError();
                _print(ss.str());
                DJV_ASSERT(f.hasError());
            }
            {
                std::stringstream ss;
                std::string text = "testing {}";
                String::Format f = String::Format(text).arg("one");
                ss << text << ": " << f.getError();
                _print(ss.str());
                DJV_ASSERT(f.hasError());
            }
            {
                std::stringstream ss;
                std::string text = "testing {0} {0}";
                String::Format f = String::Format(text).arg("one").arg("two");
                ss << text << ": " << f.getError();
                _print(ss.str());
                DJV_ASSERT(f.hasError());
            }
        }
        
    } // namespace CoreTest
} // namespace djv

