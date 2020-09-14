// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/JPEGTest.h>

#include <djvAV/JPEG.h>

#include <djvCore/ErrorFunc.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::IO;

namespace djv
{
    namespace AVTest
    {
        JPEGTest::JPEGTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::JPEGTest", tempPath, context)
        {}
        
        void JPEGTest::run()
        {
            _serialize();
        }

        void JPEGTest::_serialize()
        {
            {
                JPEG::Options options;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(options, allocator);
                JPEG::Options options2;
                fromJSON(json, options2);
                DJV_ASSERT(options == options2);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                JPEG::Options options;
                fromJSON(json, options);
                DJV_ASSERT(options == options);
            }
            catch (const std::exception& e)
            {
                _print(Error::format(e.what()));
            }
        }
        
    } // namespace AVTest
} // namespace djv

