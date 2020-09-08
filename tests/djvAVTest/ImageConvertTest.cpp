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
                auto types = Image::getTypeEnums();
                for (const auto & i : {
                    Image::Type::None,
                    Image::Type::L_U32,
                    Image::Type::LA_U32,
                    Image::Type::RGB_U32,
                    Image::Type::RGBA_U32 })
                {
                    const auto j = std::find(types.begin(), types.end(), i);
                    if (j != types.end())
                    {
                        types.erase(j);
                    }
                }
                for (const auto& i : types)
                {
                    for (const auto& j : types)
                    {
                        for (const auto& k : {
                            Image::Layout(Image::Mirror(false, false)),
                            Image::Layout(Image::Mirror(true, false)),
                            Image::Layout(Image::Mirror(false, true)),
                            Image::Layout(Image::Mirror(true, true)) })
                        {
                            const Image::Info info(64, 64, i, k);
                            auto data = Image::Data::create(info);
                            data->zero();
                            {
                                std::stringstream ss;
                                ss << i;
                                _print("Input: " + _getText(ss.str()));
                            }
                            
                            const Image::Info info2(64, 64, j);
                            auto data2 = Image::Data::create(info2);
                            
                            auto convert = Image::Convert::create(context->getSystemT<ResourceSystem>());
                            convert->process(*data, info2, *data2);
                            const Image::U8_T u8 = reinterpret_cast<const Image::U8_T*>(data2->getData())[0];
                            {
                                std::stringstream ss;
                                ss << j;
                                _print("Output: " + _getText(ss.str()));
                            }
                        }
                    }
                }
            }
        }
                
    } // namespace AVTest
} // namespace djv

