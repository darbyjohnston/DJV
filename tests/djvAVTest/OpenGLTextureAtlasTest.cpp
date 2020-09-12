// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/OpenGLTextureAtlasTest.h>

#include <djvAV/OpenGLTextureAtlas.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::OpenGL;

namespace djv
{
    namespace AVTest
    {
        OpenGLTextureAtlasTest::OpenGLTextureAtlasTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::OpenGLTextureAtlasTest", tempPath, context)
        {}
        
        void OpenGLTextureAtlasTest::run()
        {
            for (const auto count : { 1, 2, 0 })
            {
                //for (const auto type : Image::getTypeEnums())
                const auto type = Image::Type::RGBA_U8;
                {
                    {
                        std::stringstream ss;
                        ss << count;
                        std::stringstream ss2;
                        ss2 << type;
                        _print(ss.str() + "/" + _getText(ss2.str()));
                    }
                    TextureAtlas atlas(count, 64, type);
                    DJV_ASSERT(count == atlas.getTextureCount());
                    DJV_ASSERT(64 == atlas.getTextureSize());
                    DJV_ASSERT(type == atlas.getTextureType());
                    for (const auto i : atlas.getTextures())
                    {
                        {
                            std::stringstream ss;
                            ss << i;
                            _print("    ID: " + ss.str());
                        }
                    }
                    auto data = Image::Data::create(Image::Info(16, 16, type));
                    for (size_t i = 0; i < 100; ++i)
                    {
                        TextureAtlasItem item;
                        UID uid = atlas.addItem(data, item);
                        atlas.getItem(uid, item);
                    }
                    {
                        std::stringstream ss;
                        ss << std::fixed << atlas.getPercentageUsed();
                        _print("    Percentage used: " + ss.str());
                    }
                }
            }
        }

    } // namespace AVTest
} // namespace djv

