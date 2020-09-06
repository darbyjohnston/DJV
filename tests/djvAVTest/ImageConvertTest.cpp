// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/ImageConvertTest.h>

#include <djvAV/ImageConvert.h>

#include <djvCore/Context.h>
#include <djvCore/ResourceSystem.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        ImageConvertTest::ImageConvertTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::ImageConvertTest", tempPath, context)
        {}
        
        void ImageConvertTest::run()
        {
            if (auto context = getContext().lock())
            {
                const Image::Info info(64, 64, Image::Type::L_U8);
                auto data = Image::Data::create(info);
                data->getData()[0] = Image::U8Range.getMax();
                {
                    std::stringstream ss;
                    ss << "input: " << static_cast<uint16_t>(data->getData()[0]);
                    _print(ss.str());
                }
                
                const Image::Info info2(64, 64, Image::Type::RGBA_U8);
                auto data2 = Image::Data::create(info2);
                
                auto convert = Image::Convert::create(context->getSystemT<ResourceSystem>());
                convert->process(*data, info2, *data2);
                const Image::U8_T u8 = reinterpret_cast<const Image::U8_T*>(data2->getData())[0];
                {
                    std::stringstream ss;
                    ss << "output: " << static_cast<uint16_t>(u8);
                    _print(ss.str());
                }
                //DJV_ASSERT(Image::U8Range.max == u8);
            }
        }
                
    } // namespace AVTest
} // namespace djv

