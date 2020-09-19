// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImageTest/ImageTest.h>

#include <djvImage/Image.h>

using namespace djv::Core;
using namespace djv::Image;

namespace djv
{
    namespace ImageTest
    {
        ImageTest::ImageTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::ImageTest::ImageTest", tempPath, context)
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
                tags.set("a", "1");
                tags.set("b", "2");
                tags.set("c", "3");
                image->setTags(tags);
                DJV_ASSERT(tags == image->getTags());
            }
        }
                
    } // namespace ImageTest
} // namespace djv

