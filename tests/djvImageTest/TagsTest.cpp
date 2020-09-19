// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImageTest/TagsTest.h>

#include <djvImage/Tags.h>

using namespace djv::Core;
using namespace djv::Image;

namespace djv
{
    namespace ImageTest
    {
        TagsTest::TagsTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::ImageTest::TagsTest", tempPath, context)
        {}
        
        void TagsTest::run()
        {
            {
                const Tags tags;
                DJV_ASSERT(tags.isEmpty());
                DJV_ASSERT(0 == tags.getCount());
                DJV_ASSERT(tags.get().empty());
                DJV_ASSERT(!tags.contains("tag"));
                DJV_ASSERT(tags.get("tag").empty());
            }
            
            {
                Tags tags;
                const std::map<std::string, std::string> map =
                {
                    { "a", "1" },
                    { "b", "2" },
                    { "c", "3" }
                };
                tags.set(map);
                DJV_ASSERT(map == tags.get());
                tags.set("d", "4");
                DJV_ASSERT(tags.contains("d"));
                DJV_ASSERT("4" == tags.get("d"));
            }
            
            {
                Tags tags;
                const std::map<std::string, std::string> map =
                {
                    { "a", "1" },
                    { "b", "2" },
                    { "c", "3" }
                };
                tags.set(map);
                Tags tags2;
                tags2.set(map);
                DJV_ASSERT(tags == tags2);
            }
        }

    } // namespace ImageTest
} // namespace djv

