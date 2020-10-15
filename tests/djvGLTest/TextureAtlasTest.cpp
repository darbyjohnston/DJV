// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvGLTest/TextureAtlasTest.h>

#include <djvGL/TextureAtlas.h>

#include <djvImage/Data.h>

using namespace djv::Core;
using namespace djv::GL;

namespace djv
{
    namespace GLTest
    {
        TextureAtlasTest::TextureAtlasTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::GLTest::TextureAtlasTest", tempPath, context)
        {}
        
        void TextureAtlasTest::run()
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

    } // namespace GLTest
} // namespace djv

