// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/PathFuncTest.h>

#include <djvCore/ErrorFunc.h>
#include <djvCore/PathFunc.h>
#if defined(DJV_PLATFORM_MACOS) || defined(DJV_PLATFORM_LINUX)
#include <djvCore/OSFunc.h>
#endif // DJV_PLATFORM_MACOS || DJV_PLATFORM_LINUX

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        PathFuncTest::PathFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::PathFuncTest", tempPath, context)
        {}
        
        void PathFuncTest::run()
        {
            _enum();
            _util();
            _serialize();
        }
                
        void PathFuncTest::_enum()
        {
            for (auto i : FileSystem::getResourcePathEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Resource path: " + _getText(ss.str()));
            }
        }

        void PathFuncTest::_util()
        {
            {
                std::string path("/a/b/");
                FileSystem::removeTrailingSeparator(path);
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
                    const auto pieces = FileSystem::splitDir(d.path);
                    DJV_ASSERT(pieces == d.pieces);
                    const std::string path = FileSystem::joinDirs(pieces, d.seperator);
                    std::stringstream ss;
                    ss << "Split/join: " << d.path << " = " << path;
                    _print(ss.str());
                    DJV_ASSERT(d.path == path);
                }
            }
            
            {
                const FileSystem::Path path("foo");
                FileSystem::mkdir(path);
                FileSystem::rmdir(path);
            }

            {            
                const FileSystem::Path path("foo");
                try
                {
                    FileSystem::mkdir(path);
                    FileSystem::mkdir(path);
                }
                catch (const std::exception & e)
                {
                    _print(Error::format(e));
                }
                try
                {
                    FileSystem::rmdir(path);
                    FileSystem::rmdir(path);
                }
                catch (const std::exception & e)
                {
                    _print(Error::format(e));
                }
            }
            
            {
                const FileSystem::Path path = FileSystem::getAbsolute(getTempPath());
                std::stringstream ss;
                ss << "Absolute: " << path;
                _print(ss.str());
            }
            
            {
                const FileSystem::Path path = FileSystem::getAbsolute(FileSystem::Path());
                std::stringstream ss;
                ss << "Absolute: " << path;
                _print(ss.str());
            }

            {
                const FileSystem::Path path = FileSystem::getCWD();
                std::stringstream ss;
                ss << "CWD: " << path;
                _print(ss.str());
            }

            {
                const FileSystem::Path path = FileSystem::getTemp();
                std::stringstream ss;
                ss << "Temp: " << path;
                _print(ss.str());
            }

#if defined(DJV_PLATFORM_MACOS) || defined(DJV_PLATFORM_LINUX)
            for (const auto& i : std::vector<std::string>({ "TEMP", "TMP", "TMPDIR" }))
            {
                std::string prev;
                bool hasPrev = OS::getEnv(i, prev);
                OS::setEnv(i, i);
                const FileSystem::Path path = FileSystem::getTemp();
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
                const FileSystem::Path path("/a/b");
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(path, allocator);
                FileSystem::Path path2;
                fromJSON(json, path2);
                DJV_ASSERT(path == path2);
            }

            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                FileSystem::Path path;
                fromJSON(json, path);
                DJV_ASSERT(FileSystem::Path() == path);
            }
        }
                
    } // namespace CoreTest
} // namespace djv

