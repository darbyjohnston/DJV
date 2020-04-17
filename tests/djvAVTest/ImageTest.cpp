// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/ImageTest.h>

#include <djvAV/Image.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        ImageTest::ImageTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::ImageTest", context)
        {}
        
        void ImageTest::run()
        {
            {
                auto image = Image::Image::create(Image::Info(1, 2, Image::Type::RGB_U8));
                DJV_ASSERT(image->getPluginName().empty());
                const std::string name = "TIFF";
                image->setPluginName(name);
                DJV_ASSERT(name == image->getPluginName());
            }

            {
                auto image = Image::Image::create(Image::Info(1, 2, Image::Type::RGB_U8));
                DJV_ASSERT(image->getTags().isEmpty());
                Tags tags;
                tags.setTag("a", "1");
                tags.setTag("b", "2");
                tags.setTag("c", "3");
                image->setTags(tags);
                DJV_ASSERT(tags == image->getTags());
            }
        }
                
    } // namespace AVTest
} // namespace djv

