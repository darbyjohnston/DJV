// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/RapidJSONTest.h>

#include <djvCore/FileIO.h>
#include <djvCore/RapidJSON.h>

#include <rapidjson/prettywriter.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        RapidJSONTest::RapidJSONTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::RapidJSONTest", context)
        {}
        
        void RapidJSONTest::run()
        {
            _io();
            _conversion();
        }
                
        void RapidJSONTest::_io()
        {
            rapidjson::Document document;
            document.SetObject();
            auto& allocator = document.GetAllocator();
            document.AddMember("string", rapidjson::Value("1", allocator), allocator);
            //json.AddMember("double", rapidjson::Value(2.0, allocator), allocator);
            {
                rapidjson::Value value(rapidjson::kArrayType);
                value.PushBack(rapidjson::Value("1", allocator), allocator);
                value.PushBack(rapidjson::Value("2", allocator), allocator);
                value.PushBack(rapidjson::Value("3", allocator), allocator);
                document.AddMember("array", value, allocator);
            }
            {
                rapidjson::Value value(rapidjson::kObjectType);
                value.AddMember("a", rapidjson::Value("1", allocator), allocator);
                value.AddMember("b", rapidjson::Value("2", allocator), allocator);
                value.AddMember("c", rapidjson::Value("3", allocator), allocator);
                document.AddMember("object", value, allocator);
            }
            for (size_t i : { 0, 4 })
            {
                rapidjson::StringBuffer buffer;
                rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
                document.Accept(writer);

                auto io = FileSystem::FileIO::create();
                io->open("RapidJSONTest.json", FileSystem::FileIO::Mode::Write);
                io->write(buffer.GetString());
                io->open("RapidJSONTest.json", FileSystem::FileIO::Mode::Read);
                std::vector<char> buf;
                const size_t bufSize = io->getSize();
                buf.resize(bufSize);
                io->read(buf.data(), bufSize);
                const char* bufP = buf.data();

                rapidjson::Document document2;
                rapidjson::ParseResult result = document2.Parse(bufP, bufSize);
                DJV_ASSERT(result);
                DJV_ASSERT(document2 == document);
            }
        }
        
        void RapidJSONTest::_conversion()
        {
            {
                const bool value = true;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                bool value2 = false;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                bool value = false;
                fromJSON(json, value);
                DJV_ASSERT(false);                
            }
            catch (const std::exception&)
            {}
            
            {
                const int value = 1;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                int value2 = 0;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                int value = 0;
                fromJSON(json, value);
                DJV_ASSERT(false);                
            }
            catch (const std::exception&)
            {}
            
            {
                const float value = 1.F;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                float value2 = 0.F;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                float value = 0.F;
                fromJSON(json, value);
                DJV_ASSERT(false);                
            }
            catch (const std::exception&)
            {}
            
            {
                const size_t value = 1;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                size_t value2 = 0;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                size_t value = 0;
                fromJSON(json, value);
                DJV_ASSERT(false);                
            }
            catch (const std::exception&)
            {}

            {
                const std::string value = "RapidJSONTest";
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                std::string value2;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            {
                const std::string value = "\"RapidJSONTest\"";
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                std::string value2;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                std::string value;
                fromJSON(json, value);
                DJV_ASSERT(false);                
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace CoreTest
} // namespace djv

