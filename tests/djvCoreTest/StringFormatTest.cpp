// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

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

