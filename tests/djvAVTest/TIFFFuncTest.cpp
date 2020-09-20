// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/TIFFFuncTest.h>

#include <djvAV/TIFFFunc.h>

#include <djvCore/ErrorFunc.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::IO;

namespace djv
{
    namespace AVTest
    {
        TIFFFuncTest::TIFFFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::AVTest::TIFFFuncTest", tempPath, context)
        {}
        
        void TIFFFuncTest::run()
        {
            _serialize();
        }

        void TIFFFuncTest::_serialize()
        {
            {
                IO::TIFF::Options options;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(options, allocator);
                IO::TIFF::Options options2;
                fromJSON(json, options2);
                DJV_ASSERT(options == options2);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                IO::TIFF::Options options;
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

