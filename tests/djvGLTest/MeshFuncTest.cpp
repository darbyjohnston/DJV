// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvGLTest/MeshFuncTest.h>

#include <djvGL/MeshFunc.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::GL;

namespace djv
{
    namespace GLTest
    {
        MeshFuncTest::MeshFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::GLTest::MeshFuncTest", tempPath, context)
        {}
        
        void MeshFuncTest::run()
        {
            _enum();
        }
        
        void MeshFuncTest::_enum()
        {
            for (const auto& i : getVBOTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << getVertexByteCount(i);
                _print(_getText(ss.str()) + " byte count: " + ss2.str());
            }
        }
        
    } // namespace GLTest
} // namespace djv

