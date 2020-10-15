// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImageTest/DataTest.h>

#include <djvImage/Data.h>
#include <djvImage/DataFunc.h>

#include <djvCore/Memory.h>

using namespace djv::Core;
using namespace djv::Image;

namespace djv
{
    namespace ImageTest
    {
        DataTest::DataTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::ImageTest::DataTest", tempPath, context)
        {}
        
        void DataTest::run()
        {
            _data();
            _operators();
        }
                
        void DataTest::_data()
        {
            {
                auto data = Image::Data::create(Image::Info());
                DJV_ASSERT(!data->isValid());
            }
            
            {
                const Image::Info info(1, 2, Image::Type::RGB_U8);
                auto data = Image::Data::create(info);
                DJV_ASSERT(info == data->getInfo());
                DJV_ASSERT(info.size == data->getSize());
                DJV_ASSERT(info.size.w == data->getWidth());
                DJV_ASSERT(info.size.h == data->getHeight());
                DJV_ASSERT(info.size.getAspectRatio() == data->getAspectRatio());
                DJV_ASSERT(info.type == data->getType());
                DJV_ASSERT(info.getGLFormat() == data->getGLFormat());
                DJV_ASSERT(info.getGLType() == data->getGLType());
                DJV_ASSERT(info.layout == data->getLayout());
                DJV_ASSERT(data->isValid());
                DJV_ASSERT(info.getPixelByteCount() == data->getPixelByteCount());
                DJV_ASSERT(info.getScanlineByteCount() == data->getScanlineByteCount());
                DJV_ASSERT(info.getDataByteCount() == data->getDataByteCount());
                DJV_ASSERT(data->getData());
                DJV_ASSERT(data->getData(0));
                DJV_ASSERT(data->getData(0, 0));

                auto data2 = Image::Data::create(info);
                DJV_ASSERT(data->getUID() != data2->getUID());
            }
            
            {
                auto image = Image::Data::create(Image::Info(1, 2, Image::Type::RGB_U8));
                DJV_ASSERT(image->getPluginName().empty());
                const std::string name = "TIFF";
                image->setPluginName(name);
                DJV_ASSERT(name == image->getPluginName());
            }

            {
                auto image = Image::Data::create(Image::Info(1, 2, Image::Type::RGB_U8));
                DJV_ASSERT(image->getTags().isEmpty());
                Tags tags;
                tags.set("a", "1");
                tags.set("b", "2");
                tags.set("c", "3");
                image->setTags(tags);
                DJV_ASSERT(tags == image->getTags());
            }
        }
        
        void DataTest::_util()
        {
            {
                const Image::Info info(1, 2, Image::Type::RGB_U8);
                auto data = Image::Data::create(info);
                data->zero();
                DJV_ASSERT(0 == data->getData(0, 0));
            }
        }

        void DataTest::_operators()
        {
            {
                const Image::Info info(1, 2, Image::Type::RGB_U8);
                auto data = Image::Data::create(info);

                const Image::Info info2(2, 1, Image::Type::RGB_U8);
                auto data2 = Image::Data::create(info2);
                DJV_ASSERT(*data != *data2);
            }

            {
                const Image::Info info(1, 2, Image::Type::RGB_U8);
                auto data = Image::Data::create(info);
                data->zero();

                auto data2 = Image::Data::create(info);
                data2->zero();
                DJV_ASSERT(*data == *data2);

                data2->getData()[0] = 1;
                data2->getData(0)[1] = 2;
                data2->getData(0, 1)[0] = 3;
                DJV_ASSERT(*data != *data2);
            }

            {
                const Image::Info info(1, 2, Image::Type::RGB_U10);
                auto data = Image::Data::create(info);
                data->zero();

                auto data2 = Image::Data::create(info);
                data2->zero();
                DJV_ASSERT(*data == *data2);

                data2->getData()[0] = 1;
                data2->getData(0)[1] = 2;
                data2->getData(0, 1)[0] = 3;
                DJV_ASSERT(*data != *data2);
            }
        }

    } // namespace ImageTest
} // namespace djv

