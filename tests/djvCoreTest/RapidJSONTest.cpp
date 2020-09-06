// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/RapidJSONTest.h>

#include <djvCore/FileIO.h>
#include <djvCore/Path.h>
#include <djvCore/RapidJSON.h>
#include <djvCore/RapidJSONTemplates.h>
#include <djvCore/StringFormat.h>

#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        RapidJSONTest::RapidJSONTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest(
                "djv::CoreTest::RapidJSONTest",
                FileSystem::Path(tempPath, "RapidJSONTest"),
                context)
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
            document.AddMember("string", toJSON("1", allocator), allocator);
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
            rapidjson::StringBuffer buffer;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            document.Accept(writer);

            auto io = FileSystem::FileIO::create();
            io->open(
                FileSystem::Path(getTempPath(), "file.json").get(),
                FileSystem::FileIO::Mode::Write);
            io->write(buffer.GetString());
            io->open(
                FileSystem::Path(getTempPath(), "file.json").get(),
                FileSystem::FileIO::Mode::Read);
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
                const int64_t value = 1;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                int64_t value2 = 0;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                int64_t value = 0;
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
                const double value = 1.0;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                double value2 = 0.F;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                double value = 0.0;
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
            
            {
                const std::vector<std::string> value({ "one", "two", "three" });
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                std::vector<std::string> value2;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                std::vector<std::string> value;
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const std::map<std::string, std::string> value({
                    { "one", "1" },
                    { "two", "2" },
                    { "three", "3" } });
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                std::map<std::string, std::string> value2;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kArrayType);
                std::map<std::string, std::string> value;
                fromJSON(json, value);
                DJV_ASSERT(false);                
            }
            catch (const std::exception&)
            {}
            
            {
                std::string buf =
                    "{\n"
                    "    error\n"
                    "}\n";
                rapidjson::Document document;
                rapidjson::ParseResult result = document.Parse(buf.c_str(), buf.size());
                DJV_ASSERT(!result);
                size_t line = 0;
                size_t character = 0;
                RapidJSON::errorLineNumber(buf.c_str(), buf.size(), result.Offset(), line, character);
                _print(String::Format("{0} {1} {2}, {3} {4}").
                    arg(rapidjson::GetParseError_En(result.Code())).
                    arg("Line").
                    arg(line).
                    arg("Character").
                    arg(character));
            }
        }
        
    } // namespace CoreTest
} // namespace djv

