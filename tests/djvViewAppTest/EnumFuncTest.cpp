// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewAppTest/EnumFuncTest.h>

#include <djvViewApp/EnumFunc.h>

using namespace djv::Core;
using namespace djv::ViewApp;

namespace djv
{
    namespace ViewAppTest
    {        
        EnumFuncTest::EnumFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::ViewAppTest::EnumFuncTest", tempPath, context)
        {}
        
        void EnumFuncTest::run()
        {
        }

    } // namespace ViewAppTest
} // namespace djv

