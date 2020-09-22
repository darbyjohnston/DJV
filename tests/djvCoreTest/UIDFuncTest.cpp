// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/UIDFuncTest.h>

#include <djvCore/StringFormat.h>
#include <djvCore/UIDFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        UIDFuncTest::UIDFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::CoreTest::UIDFuncTest", tempPath, context)
        {}
        
        void UIDFuncTest::run()
        {
            uint64_t a = createUID();
            uint64_t b = createUID();
            uint64_t c = createUID();
            _print(String::Format(
                "{0}, {1}, {2}").
                arg(static_cast<size_t>(a)).
                arg(static_cast<size_t>(b)).
                arg(static_cast<size_t>(c)));
            DJV_ASSERT(a != b);
            DJV_ASSERT(b != c);
        }
        
    } // namespace CoreTest
} // namespace djv

