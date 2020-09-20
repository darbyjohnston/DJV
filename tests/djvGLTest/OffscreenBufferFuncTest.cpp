// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvGLTest/OffscreenBufferFuncTest.h>

#include <djvGL/OffscreenBufferFunc.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::GL;

namespace djv
{
    namespace GLTest
    {
        OffscreenBufferFuncTest::OffscreenBufferFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::GLTest::OffscreenBufferFuncTest", tempPath, context)
        {}
        
        void OffscreenBufferFuncTest::run()
        {
            _enum();
        }

        void OffscreenBufferFuncTest::_enum()
        {
            for (const auto& i : getOffscreenDepthTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Depth type: " + _getText(ss.str()));
            }
            
            for (const auto& i : getOffscreenSamplingEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Offscreen sampling: " + _getText(ss.str()));
            }
        }
        
    } // namespace GLTest
} // namespace djv

