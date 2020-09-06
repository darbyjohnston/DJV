// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/TagsTest.h>

#include <djvAV/Tags.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        TagsTest::TagsTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::TagsTest", tempPath, context)
        {}
        
        void TagsTest::run()
        {
            {
                const Tags tags;
                DJV_ASSERT(tags.isEmpty());
                DJV_ASSERT(0 == tags.getCount());
                DJV_ASSERT(tags.getTags().empty());
                DJV_ASSERT(!tags.hasTag("tag"));
                DJV_ASSERT(tags.getTag("tag").empty());
            }
            
            {
                Tags tags;
                const std::map<std::string, std::string> map =
                {
                    { "a", "1" },
                    { "b", "2" },
                    { "c", "3" }
                };
                tags.setTags(map);
                DJV_ASSERT(map == tags.getTags());
                tags.setTag("d", "4");
                DJV_ASSERT(tags.hasTag("d"));
                DJV_ASSERT("4" == tags.getTag("d"));
            }
            
            {
                Tags tags;
                const std::map<std::string, std::string> map =
                {
                    { "a", "1" },
                    { "b", "2" },
                    { "c", "3" }
                };
                tags.setTags(map);
                Tags tags2;
                tags2.setTags(map);
                DJV_ASSERT(tags == tags2);
            }
        }

    } // namespace AVTest
} // namespace djv

