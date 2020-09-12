// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/OpenGLTextureTest.h>

#include <djvAV/OpenGLTexture.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::OpenGL;

namespace djv
{
    namespace AVTest
    {
        OpenGLTextureTest::OpenGLTextureTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::OpenGLTextureTest", tempPath, context)
        {}
        
        void OpenGLTextureTest::run()
        {
            for (const auto& size : {
                Image::Size(64, 64),
                Image::Size(0, 0) })
            {
                for (const auto type : Image::getTypeEnums())
                {
                    const Image::Info info(size, type);
                    {
                        std::stringstream ss;
                        ss << size;
                        std::stringstream ss2;
                        ss2 << type;
                        _print(_getText(ss.str()) + " " + _getText(ss2.str()));
                    }
                    auto texture = Texture::create(info);
                    DJV_ASSERT(texture->getInfo() == info);
                    {
                        std::stringstream ss;
                        ss << texture->getID();
                        _print("    ID: " + ss.str());
                    }
                    const Image::Info info2(size.w / 2, size.h / 2, type);
                    texture->set(info2);
                    texture->set(info2);
                    auto data = Image::Data::create(info2);
                    texture->copy(*data);
                    auto data2 = Image::Data::create(Image::Info(4, 4, type));
                    texture->copy(*data2, 4, 4);
                    texture->bind();
                    {
                        std::stringstream ss;
                        ss << texture->getID();
                        _print("    Internal format: " + ss.str());
                    }
                }
            }
        }

    } // namespace AVTest
} // namespace djv

