// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/StringTest.h>

#include <djvCore/String.h>

#include <iostream>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        StringTest::StringTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::StringTest", tempPath, context)
        {}
        
        void StringTest::run()
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
                const std::list<std::string> data({ "a", "b", "c" });
                DJV_ASSERT("abc" == String::joinList(data));
                DJV_ASSERT("a/b/c" == String::joinList(data, '/'));
                DJV_ASSERT("a/b/c" == String::joinList(data, "/"));
            }
            
            {
                const std::set<std::string> data({ "a", "b", "c" });
                DJV_ASSERT("abc" == String::joinSet(data));
                DJV_ASSERT("a/b/c" == String::joinSet(data, '/'));
                DJV_ASSERT("a/b/c" == String::joinSet(data, "/"));
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
                    ss << "Escape/unescape: " << d << " = " << b;
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

