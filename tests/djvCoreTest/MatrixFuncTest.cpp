// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/MatrixFuncTest.h>

#include <djvCore/MatrixFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        MatrixFuncTest::MatrixFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::MatrixFuncTest", tempPath, context)
        {}
        
        void MatrixFuncTest::run()
        {
            {
                const glm::mat4x4 m(1.F);
                std::stringstream ss;
                ss << m;
                glm::mat4x4 m2(0.F);
                ss >> m2;
                DJV_ASSERT(m == m2);
            }

            try
            {
                glm::mat4x4 m(0.F);
                std::stringstream ss;
                ss >> m;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace CoreTest
} // namespace djv

