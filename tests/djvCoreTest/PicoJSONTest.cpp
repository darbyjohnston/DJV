// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/PicoJSONTest.h>

#include <djvCore/FileIO.h>
#include <djvCore/PicoJSON.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        PicoJSONTest::PicoJSONTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::PicoJSONTest", context)
        {}
        
        void PicoJSONTest::run()
        {
            _io();
            _conversion();
        }
                
        void PicoJSONTest::_io()
        {
            picojson::value json(picojson::object_type, true);
            json.get<picojson::object>()["string"] = picojson::value("1");
            //json.get<picojson::object>()["double"] = picojson::value(2.0);
            {
                picojson::value value(picojson::array_type, true);
                value.get<picojson::array>().push_back(picojson::value("1"));
                value.get<picojson::array>().push_back(picojson::value("2"));
                value.get<picojson::array>().push_back(picojson::value("3"));
                json.get<picojson::object>()["array"] = value;
            }
            {
                picojson::value value(picojson::object_type, true);
                value.get<picojson::object>()["a"] = picojson::value("1");
                value.get<picojson::object>()["b"] = picojson::value("2");
                value.get<picojson::object>()["c"] = picojson::value("3");
                json.get<picojson::object>()["object"] = value;
            }
            for (size_t i : { 0, 4 })
            {
                for (bool j : { false, true })
                {
                    auto io = FileSystem::FileIO::create();
                    io->open("PicoJSONTest.json", FileSystem::FileIO::Mode::Write);
                    PicoJSON::write(json, io, i, j);
                    
                    io->open("PicoJSONTest.json", FileSystem::FileIO::Mode::Read);
                    std::vector<char> buf;
                    const size_t fileSize = io->getSize();
                    buf.resize(fileSize);
                    io->read(buf.data(), fileSize);
                    const char* bufP = buf.data();
                    const char* bufEnd = bufP + fileSize;
                    picojson::value json2;
                    std::string error;
                    picojson::parse(json2, bufP, bufEnd, &error);
                    DJV_ASSERT(error.empty());
                    DJV_ASSERT(json == json2);
                }
            }
        }
        
        void PicoJSONTest::_conversion()
        {
            {
                const bool value = true;
                auto json = toJSON(value);
                bool value2 = false;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = picojson::value(picojson::object_type, true);
                bool value = false;
                fromJSON(json, value);
                DJV_ASSERT(false);                
            }
            catch (const std::exception&)
            {}
            
            {
                const int value = 1;
                auto json = toJSON(value);
                int value2 = 0;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = picojson::value(picojson::object_type, true);
                int value = 0;
                fromJSON(json, value);
                DJV_ASSERT(false);                
            }
            catch (const std::exception&)
            {}
            
            {
                const float value = 1.F;
                auto json = toJSON(value);
                float value2 = 0.F;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = picojson::value(picojson::object_type, true);
                float value = 0.F;
                fromJSON(json, value);
                DJV_ASSERT(false);                
            }
            catch (const std::exception&)
            {}
            
            {
                const size_t value = 1;
                auto json = toJSON(value);
                size_t value2 = 0;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = picojson::value(picojson::object_type, true);
                size_t value = 0;
                fromJSON(json, value);
                DJV_ASSERT(false);                
            }
            catch (const std::exception&)
            {}
            
            {
                const std::string value = "PicoJSONTest";
                auto json = toJSON(value);
                std::string value2;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = picojson::value(picojson::object_type, true);
                std::string value;
                fromJSON(json, value);
                DJV_ASSERT(false);                
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace CoreTest
} // namespace djv

