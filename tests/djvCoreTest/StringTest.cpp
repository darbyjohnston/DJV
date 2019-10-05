//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvCoreTest/StringTest.h>

#include <djvCore/String.h>

#include <iostream>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        StringTest::StringTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::StringTest", context)
        {}
        
        void StringTest::run(const std::vector<std::string>& args)
        {
            {
                std::vector<std::pair<std::string, std::vector<std::string> > > data =
                {
                    { "",      {} },
                    { "a/b",   { "a", "b" } },
                    { "a/",    { "a" } },
                    { "/b",    { "b" } },
                    { "a/b/c", { "a", "b", "c" } }
                };
                for (const auto& d : data)
                {
                    const auto s = String::split(d.first.c_str(), '/');
                    DJV_ASSERT(s == d.second);
                }
                for (const auto& d : data)
                {
                    const auto s = String::split(d.first, '/');
                    DJV_ASSERT(s == d.second);
                }
            }
            
            {
                std::vector<std::pair<std::string, std::vector<std::string> > > data =
                {
                    { "",      {} },
                    { "a//b",   { "a", "", "b" } },
                    { "a//",    { "a", "" } },
                    { "//b",    { "", "b" } },
                    { "a//b//c", { "a", "", "b", "", "c" } }
                };
                for (const auto& d : data)
                {
                    const auto s = String::split(d.first.c_str(), d.first.size(), '/', true);
                    DJV_ASSERT(s == d.second);
                }
                for (const auto& d : data)
                {
                    const auto s = String::split(d.first, '/', true);
                    DJV_ASSERT(s == d.second);
                }
            }
            
            {
                std::vector<std::pair<std::string, std::vector<std::string> > > data =
                {
                    { "",      {} },
                    { "a/b|",  { "a", "b" } },
                    { "|a/",   { "a" } },
                    { "/b|",   { "b" } },
                    { "a/b|c", { "a", "b", "c" } }
                };
                for (const auto& d : data)
                {
                    const auto s = String::split(d.first, { '/', '|' });
                    DJV_ASSERT(s == d.second);
                }
            }
            
            {
                std::vector<std::pair<std::string, std::vector<std::string> > > data =
                {
                    { "",      {} },
                    { "a/|b",  { "a", "", "b" } },
                    { "a/|",   { "a", "" } },
                    { "/|b",   { "", "b" } },
                    { "a//b||c", { "a", "", "b", "", "c" } }
                };
                for (const auto& d : data)
                {
                    const auto s = String::split(d.first, { '/', '|' }, true);
                    DJV_ASSERT(s == d.second);
                }
            }
            
            {
                std::vector<std::pair<std::vector<std::string>, std::string > > data =
                {
                    { {},                "" },
                    { { "a", "b" },      "ab" },
                    { { "a", "b", "c" }, "abc" }
                };
                for (const auto& d : data)
                {
                    const auto s = String::join(d.first);
                    DJV_ASSERT(s == d.second);
                }
            }
            
            {
                std::vector<std::pair<std::vector<std::string>, std::string > > data =
                {
                    { {},                "" },
                    { { "a", "b" },      "a/b" },
                    { { "a", "b", "c" }, "a/b/c" }
                };
                for (const auto& d : data)
                {
                    const auto s = String::join(d.first, '/');
                    DJV_ASSERT(s == d.second);
                }
                for (const auto& d : data)
                {
                    const auto s = String::join(d.first, "/");
                    DJV_ASSERT(s == d.second);
                }
            }
            
            {
                std::vector<std::pair<std::string, std::string> > data =
                {
                    { "", "" },
                    { "upper", "UPPER" },
                    { "UPPER", "UPPER" }
                };
                for (const auto & d : data)
                {
                    DJV_ASSERT(String::toUpper(d.first) == d.second);
                }
            }
            
            {
                std::vector<std::pair<std::string, std::string> > data =
                {
                    { "", "" },
                    { "LOWER", "lower" },
                    { "lower", "lower" }
                };
                for (const auto & d : data)
                {
                    DJV_ASSERT(String::toLower(d.first) == d.second);
                }
            }
            
            {
                std::vector<std::pair<int, std::string> > data =
                {
                    { 0, "" },
                    { 1, "    " },
                    { 2, "        " }
                };
                for (const auto & d : data)
                {
                    DJV_ASSERT(String::indent(d.first) == d.second);
                }
            }
            
            {
                std::vector<std::pair<std::string, std::string> > data =
                {
                    { "", "" },
                    { "test\n", "test" },
                    { "test\r", "test" }
                };
                for (const auto & d : data)
                {
                    auto s = d.first;
                    String::removeTrailingNewline(s);
                    DJV_ASSERT(s == d.second);
                }
            }
            
            {
                struct Data
                {
                    std::string value;
                    std::string expression;
                    bool result;
                };
                std::vector<Data> data =
                {
                    { "", "", true },
                    { "test", ".*", true },
                    { "test", "t", true },
                    { "test", "z", false },
                    { "test", "[", false }
                };
                for (const auto & d : data)
                {
                    DJV_ASSERT(String::match(d.value, d.expression) == d.result);
                }
            }
            
            {
                struct Data
                {
                    int value;
                    std::string string;
                    size_t maxLen;
                    size_t size;
                };
                std::vector<Data> data =
                {
                    { 0, "0", String::cStringLength, 1 },
                    { 10, "10", String::cStringLength, 2 },
                    { 100, "100", String::cStringLength, 3 },
                    { -100, "-100", String::cStringLength, 4 }
                };
                for (const auto & d : data)
                {
                    char buf[String::cStringLength];
                    DJV_ASSERT(String::intToString(d.value, buf, d.maxLen) == d.size);
                    DJV_ASSERT(d.string == std::string(buf));
                }
            }
            
            {
                struct Data
                {
                    std::string string;
                    int value;
                };
                std::vector<Data> data =
                {
                    { "-100", -100 },
                    { "-10", -10 },
                    { "", 0 },
                    { "10", 10 },
                    { "100", 100 },
                    { "+100", 100 }
                };
                for (const auto & d : data)
                {
                    int value = -1;
                    String::fromString(d.string.c_str(), d.string.size(), value);
                    DJV_ASSERT(d.value == value);
                }
            }
            
            {
                struct Data
                {
                    std::string string;
                    int64_t value;
                };
                std::vector<Data> data =
                {
                    { "-100", -100 },
                    { "-10", -10 },
                    { "", 0 },
                    { "10", 10 },
                    { "100", 100 },
                    { "+100", 100 }
                };
                for (const auto & d : data)
                {
                    int64_t value = -1;
                    String::fromString(d.string.c_str(), d.string.size(), value);
                    DJV_ASSERT(d.value == value);
                }
            }
            
            {
                struct Data
                {
                    std::string string;
                    size_t value;
                };
                std::vector<Data> data =
                {
                    { "", 0 },
                    { "10", 10 },
                    { "100", 100 }
                };
                for (const auto & d : data)
                {
                    size_t value = size_t(-1);
                    String::fromString(d.string.c_str(), d.string.size(), value);
                    DJV_ASSERT(d.value == value);
                }
            }
            
            {
                struct Data
                {
                    std::string string;
                    float value;
                };
                std::vector<Data> data =
                {
                    { "-100.5", -100.5F },
                    { "-10.5", -10.5F },
                    { "", 0.F },
                    { "10.5", 10.5F },
                    { "+100.5", 100.5F },
                    { "1E-1", .1F }
                };
                for (const auto & d : data)
                {
                    float value = -1;
                    String::fromString(d.string.c_str(), d.string.size(), value);
                    DJV_ASSERT(d.value == value);
                }
            }
            
            {
                std::vector<std::string> data =
                {
                    { "" },
                    { "Hello world!" }
                };
                for (const auto & d : data)
                {
                    DJV_ASSERT(String::fromWide(String::toWide(d)) == d);
                }
            }
            
            {
                std::vector<std::string> data =
                {
                    "",
                    "\\",
                    "\\\\",
                    "\\a\\b",
                    "\\\\a\\b",
                };
                for (const auto & d : data)
                {
                    const auto a = String::escape(d);
                    const auto b = String::unescape(a);
                    std::stringstream ss;
                    ss << "escape/unescape: " << d << " = " << b;
                    _print(ss.str());
                    DJV_ASSERT(d == b);
                }
            }
            
            {
                _print(String::getAlphabetLower());
                _print(String::getAlphabetUpper());
                for (const auto& i : String::getTestNames())
                {
                    _print(i);
                }
                for (const auto& i : String::getRandomNames(10))
                {
                    _print(i);
                }
                _print(String::getRandomName());
                for (const auto& i : String::getTestSentences())
                {
                    _print(i);
                }
                _print(String::getRandomSentence());
                for (const auto& i : String::getRandomSentences(10))
                {
                    _print(i);
                }
                for (auto i : { 1, 10, 100 })
                {
                    _print(String::getRandomText(i));
                }
            }
        }
        
    } // namespace CoreTest
} // namespace djv

