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
        StringFormatTest::StringFormatTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::StringFormatTest", tempPath, context)
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
                const std::string text = "testing";
                const std::string result = String::Format(text);
                ss << text << ": " << result;
                _print(ss.str());
                DJV_ASSERT(result == "testing");
            }
            
            {
                std::stringstream ss;
                const std::string text = "testing {0}";
                const std::string result = String::Format(text).arg("testing");
                ss << text << ": " << result;
                _print(ss.str());
                DJV_ASSERT(result == "testing testing");
            }
            
            {
                std::stringstream ss;
                const std::string text = "testing {0} {1} {2}";
                const std::string result = String::Format(text).arg("one").arg("two").arg("three");
                ss << text << ": " << result;
                _print(ss.str());
                DJV_ASSERT(result == "testing one two three");
            }
            
            {
                std::stringstream ss;
                const std::string text = "testing {2} {1} {0}";
                const std::string result = String::Format(text).arg("one").arg("two").arg("three");
                ss << text << ": " << result;
                _print(ss.str());
                DJV_ASSERT(result == "testing three two one");
            }
            
            {
                std::stringstream ss;
                const std::string text = "testing {0} {1} {2}";
                const std::string result = String::Format(text).arg(1).arg(2).arg(3);
                ss << text << ": " << result;
                _print(ss.str());
                DJV_ASSERT(result == "testing 1 2 3");
            }
            
            {
                std::stringstream ss;
                const std::string text = "testing {0} {1} {2}";
                const std::string result = String::Format(text).arg(1.F).arg(2.F).arg(3.F);
                ss << text << ": " << result;
                _print(ss.str());
                DJV_ASSERT(result == "testing 1 2 3");
            }
            
            {
                std::stringstream ss;
                const std::string text = "testing";
                const String::Format f = String::Format(text).arg("one");
                ss << text << ": " << f.getError();
                _print(ss.str());
                DJV_ASSERT(f.hasError());
            }
            
            {
                std::stringstream ss;
                const std::string text = "testing { 0 }";
                const String::Format f = String::Format(text).arg("one");
                ss << text << ": " << f.getError();
                _print(ss.str());
                DJV_ASSERT(f.hasError());
            }
            
            {
                std::stringstream ss;
                const std::string text = "testing {}";
                const String::Format f = String::Format(text).arg("one");
                ss << text << ": " << f.getError();
                _print(ss.str());
                DJV_ASSERT(f.hasError());
            }
            
            {
                std::stringstream ss;
                const std::string text = "testing {0} {0}";
                const String::Format f = String::Format(text).arg("one").arg("two");
                ss << text << ": " << f.getError();
                _print(ss.str());
                DJV_ASSERT(f.hasError());
            }
        }
        
    } // namespace CoreTest
} // namespace djv

