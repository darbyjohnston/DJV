// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/CacheTest.h>

#include <djvCore/Cache.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        CacheTest::CacheTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::CacheTest", tempPath, context)
        {}
        
        void CacheTest::run()
        {
            {
                Memory::Cache<int, std::string> cache;
                cache.add(1, "a");
                cache.add(2, "b");
                cache.add(3, "c");
                DJV_ASSERT(cache.getKeys() == std::vector<int>({ 1, 2, 3 }));
                DJV_ASSERT(cache.getValues() == std::vector<std::string>({ "a", "b", "c" }));
                std::string key;
                DJV_ASSERT(!cache.get(0, key));
            }
            
            {
                Memory::Cache<int, std::string> cache;
                cache.setMax(2);
                cache.add(1, "a");
                cache.add(2, "b");
                cache.add(3, "c");
                std::string value;
                cache.get(2, value);
                DJV_ASSERT(value == "b");
                DJV_ASSERT(cache.getKeys() == std::vector<int>({ 2, 3 }));
                DJV_ASSERT(cache.getValues() == std::vector<std::string>({ "b", "c" }));
            }
        }
        
    } // namespace CoreTest
} // namespace djv

