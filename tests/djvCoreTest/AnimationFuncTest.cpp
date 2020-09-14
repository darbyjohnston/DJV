// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/AnimationFuncTest.h>

#include <djvCore/AnimationFunc.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        AnimationFuncTest::AnimationFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::AnimationFuncTest", tempPath, context)
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
        
    } // namespace CoreTest
} // namespace djv

