// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvMathTest/MatrixTest.h>

#include <djvMath/Matrix.h>

using namespace djv::Core;

namespace djv
{
    namespace MathTest
    {
        MatrixTest::MatrixTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::MathTest::MatrixTest", tempPath, context)
        {}
        
        void MatrixTest::run()
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
        
    } // namespace MathTest
} // namespace djv

