// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/PathFuncTest.h>

#include <djvSystem/PathFunc.h>

#include <djvCore/ErrorFunc.h>
#if defined(DJV_PLATFORM_MACOS) || defined(DJV_PLATFORM_LINUX)
#include <djvCore/OSFunc.h>
#endif // DJV_PLATFORM_MACOS || DJV_PLATFORM_LINUX

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        PathFuncTest::PathFuncTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::SystemTest::PathFuncTest", tempPath, context)
        {}
        
        void PathFuncTest::run()
        {
            _enum();
            _util();
            _serialize();
        }
                
        void PathFuncTest::_enum()
        {
            for (auto i : File::getResourcePathEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Resource path: " + _getText(ss.str()));
            }
            
            for (auto i : File::getDirectoryShortcutEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Directory shortcut: " + _getText(ss.str()));
            }
        }

        void PathFuncTest::_util()
        {
            {
                std::string path("/a/b/");
                File::removeTrailingSeparator(path);
                DJV_ASSERT("/a/b" == path);
            }
            
            {
                struct Data
                {
                    std::string              path;
                    char                     seperator;
                    std::vector<std::string> pieces;
                };
                std::vector<Data> data =
                {
                    { "a", '/', { "a" } },
                    { "a/b", '/', { "a", "b" } },
                    { "/a/b", '/', { "/", "a", "b" } },
                    { "/", '/', { "/" } },
                    { "a\\b", '\\', { "a", "b" } },
                    { "\\a\\b", '\\', { "\\", "a", "b" } },
                    { "a:\\b", '\\', { "a:", "b" } },
                    { "\\", '\\', { "\\" } },
                    { "\\\\a", '\\', { "\\\\a" } },
                    { "\\\\a\\b", '\\', { "\\\\a", "b" } }
                };
                for (const auto & d : data)
                {
                    const auto pieces = File::splitDir(d.path);
                    DJV_ASSERT(pieces == d.pieces);
                    const std::string path = File::joinDirs(pieces, d.seperator);
                    std::stringstream ss;
                    ss << "Split/join: " << d.path << " = " << path;
                    _print(ss.str());
                    DJV_ASSERT(d.path == path);
                }
            }
            
            {
                const File::Path path("foo");
                File::mkdir(path);
                File::rmdir(path);
            }

            {            
                const File::Path path("foo");
                try
                {
                    File::mkdir(path);
                    File::mkdir(path);
                }
                catch (const std::exception & e)
                {
                    _print(Error::format(e));
                }
                try
                {
                    File::rmdir(path);
                    File::rmdir(path);
                }
                catch (const std::exception & e)
                {
                    _print(Error::format(e));
                }
            }
            
            {
                const File::Path path = File::getAbsolute(getTempPath());
                std::stringstream ss;
                ss << "Absolute: " << path;
                _print(ss.str());
            }
            
            {
                const File::Path path = File::getAbsolute(File::Path());
                std::stringstream ss;
                ss << "Absolute: " << path;
                _print(ss.str());
            }

            {
                const File::Path path = File::getCWD();
                std::stringstream ss;
                ss << "CWD: " << path;
                _print(ss.str());
            }

            {
                const File::Path path = File::getTemp();
                std::stringstream ss;
                ss << "Temp: " << path;
                _print(ss.str());
            }

            for (auto i : File::getDirectoryShortcutEnums())
            {
                _print("Directory shortcut path: " + File::getPath(i).get());
            }

#if defined(DJV_PLATFORM_MACOS) || defined(DJV_PLATFORM_LINUX)
            for (const auto& i : std::vector<std::string>({ "TEMP", "TMP", "TMPDIR" }))
            {
                std::string prev;
                bool hasPrev = OS::getEnv(i, prev);
                OS::setEnv(i, i);
                const File::Path path = File::getTemp();
                std::stringstream ss;
                ss << "Temp: " << path;
                _print(ss.str());
                if (hasPrev)
                {
                    OS::setEnv(i, prev);
                }
                else
                {
                    OS::clearEnv(i);
                }
            }
#endif // DJV_PLATFORM_MACOS || DJV_PLATFORM_LINUX
        }
                
        void PathFuncTest::_serialize()
        {
            {
                const File::Path path("/a/b");
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(path, allocator);
                File::Path path2;
                fromJSON(json, path2);
                DJV_ASSERT(path == path2);
            }

            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                File::Path path;
                fromJSON(json, path);
                DJV_ASSERT(File::Path() == path);
            }
        }
                
    } // namespace SystemTest
} // namespace djv

