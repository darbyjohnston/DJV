// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/AnimationFuncTest.h>

#include <djvSystem/AnimationFunc.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        AnimationFuncTest::AnimationFuncTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::SystemTest::AnimationFuncTest", tempPath, context)
        {}
        
        void AnimationFuncTest::run()
        {
            for (auto i : Animation::getTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Type: " + _getText(ss.str()));
                for (size_t j = 0; j <= 10; ++j)
                {
                    const float v = j / static_cast<float>(10);
                    std::stringstream ss;
                    ss << "    " << v << " = " << Animation::getFunction(i)(v);
                    _print(ss.str());
                }
            }
        }
        
    } // namespace SystemTest
} // namespace djv

