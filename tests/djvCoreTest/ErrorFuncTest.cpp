// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/ErrorFuncTest.h>

#include <djvCore/ErrorFunc.h>

#include <sstream>
#include <stdexcept>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        ErrorFuncTest::ErrorFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::ErrorFuncTest", tempPath, context)
        {}
        
        void ErrorFuncTest::run()
        {
            {
                std::stringstream ss;
                ss << Error::format(std::invalid_argument("This is an error message."));
                _print(ss.str());
            }
            
#if defined(DJV_PLATFORM_WINDOWS)
            {
                std::stringstream ss;
                ss << "last error: " << Error::getLastError();
                _print(ss.str());
            }
#endif // DJV_PLATFORM_WINDOWS
        }
        
    } // namespace CoreTest
} // namespace djv

