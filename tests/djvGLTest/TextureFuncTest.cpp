// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvGLTest/TextureFuncTest.h>

#include <djvGL/TextureFunc.h>

#include <djvImage/PixelFunc.h>

using namespace djv::Core;
using namespace djv::GL;

namespace djv
{
    namespace GLTest
    {
        TextureFuncTest::TextureFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::GLTest::TextureFuncTest", tempPath, context)
        {}
        
        void TextureFuncTest::run()
        {
            for (const auto type : Image::getTypeEnums())
            {
                std::stringstream ss;
                ss << type;
                std::stringstream ss2;
                ss2 << getInternalFormat2D(type);
                _print(_getText(ss.str()) + " internal format 2D: " + ss2.str());
            }
        }

    } // namespace GLTest
} // namespace djv

