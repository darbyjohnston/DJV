// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvOCIOTest/OCIOSystemFuncTest.h>

#include <djvOCIO/OCIOSystemFunc.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::OCIO;

namespace djv
{
    namespace OCIOTest
    {
        OCIOSystemFuncTest::OCIOSystemFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::OCIOTest::OCIOSystemFuncTest", tempPath, context)
        {}
        
        void OCIOSystemFuncTest::run()
        {
            _enum();
            _serialize();
        }
        
        void OCIOSystemFuncTest::_enum()
        {
            for (const auto& i : OCIO::getConfigModeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Config mode: " + _getText(ss.str()));
            }
        }
        
        namespace
        {
            OCIO::Config createConfig()
            {
                OCIO::Config config;
                config.fileName = "fileName";
                config.name = "name";
                config.display = "display";
                config.view = "view";
                config.imageColorSpaces = { { "PNG", "sRGB" }, { "DPX", "Cineon" } };
                return config;
            }

        } // namespace

        void OCIOSystemFuncTest::_serialize()
        {
            {
                OCIO::ConfigMode value = OCIO::ConfigMode::First;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                OCIO::ConfigMode value2;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            try
            {
                auto json = rapidjson::Value();
                OCIO::ConfigMode value;
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            
            catch (const std::exception&)
            {}
            {
                OCIO::Config config = createConfig();
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(config, allocator);
                OCIO::Config config2;
                fromJSON(json, config2);
                DJV_ASSERT(config == config2);
            }

            try
            {
                auto json = rapidjson::Value();
                OCIO::Config config;
                fromJSON(json, config);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }

    } // namespace OCIOTest
} // namespace djv

