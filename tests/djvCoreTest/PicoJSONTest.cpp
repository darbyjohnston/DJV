//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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
        
        void PicoJSONTest::run(const std::vector<std::string>& args)
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
                    FileSystem::FileIO io;
                    io.open("PicoJSONTest.json", FileSystem::FileIO::Mode::Write);
                    PicoJSON::write(json, io, i, j);
                    
                    io.open("PicoJSONTest.json", FileSystem::FileIO::Mode::Read);
                    std::vector<char> buf;
                    const size_t fileSize = io.getSize();
                    buf.resize(fileSize);
                    io.read(buf.data(), fileSize);
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

