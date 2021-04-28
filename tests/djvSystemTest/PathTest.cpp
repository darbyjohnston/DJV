// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/PathTest.h>

#include <djvSystem/Path.h>

#include <djvCore/Error.h>
#if defined(DJV_PLATFORM_MACOS) || defined(DJV_PLATFORM_LINUX)
#include <djvCore/OS.h>
#endif // DJV_PLATFORM_MACOS || DJV_PLATFORM_LINUX

#include <sstream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        PathTest::PathTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::SystemTest::PathTest", tempPath, context)
        {}
        
        void PathTest::run()
        {
            _path();
            _split();
            _operators();
            _enum();
            _util();
            _serialize();
        }

        void PathTest::_path()
        {
            {
                const File::Path path;
                DJV_ASSERT(path.isEmpty());
            }

            {
                File::Path path("/a/b");
                DJV_ASSERT(!path.isEmpty());
                DJV_ASSERT(path.cdUp('/'));
                DJV_ASSERT("/a" == path.get());
                DJV_ASSERT(path.cdUp('/'));
                DJV_ASSERT("/" == path.get());
                DJV_ASSERT(!path.cdUp('/'));
                path.append("a", '/');
                path.append("b", '/');
                DJV_ASSERT("/a/b" == path.get());
                path.setDirectoryName("/a/");
                path.setFileName("b");
                DJV_ASSERT("/a/b" == path.get());
                path.setNumber("0001");
                DJV_ASSERT("/a/b0001" == path.get());
                path.setExtension(".ext");
                DJV_ASSERT("/a/b0001.ext" == path.get());
                path.setBaseName("c");
                DJV_ASSERT("/a/c0001.ext" == path.get());
            }

            {
                DJV_ASSERT(File::Path::isSeparator('/'));
                DJV_ASSERT('/' == File::Path::getSeparator(File::PathSeparator::Unix));
                {
                    std::stringstream ss;
                    ss << "Current separator: " << File::Path::getCurrentSeparator();
                    _print(ss.str());
                }
            }
            
            {
                DJV_ASSERT(!File::Path().isRoot());
                DJV_ASSERT(File::Path("/").isRoot());
                DJV_ASSERT(!File::Path("/tmp").isRoot());
                DJV_ASSERT(File::Path("C:\\").isRoot());
                DJV_ASSERT(!File::Path("C:\\tmp").isRoot());
            }

            {
                DJV_ASSERT(!File::Path().isServer());
                DJV_ASSERT(!File::Path("\\\\").isServer());
                DJV_ASSERT(File::Path("\\\\server").isServer());
                DJV_ASSERT(File::Path("\\\\server\\").isServer());
                DJV_ASSERT(!File::Path("\\\\server\\share").isServer());
            }
        }
        
        void PathTest::_split()
        {
            struct Data
            {
                Data(
                    const std::string & value,
                    const std::string & directoryName,
                    const std::string & fileName,
                    const std::string & baseName,
                    const std::string & number,
                    const std::string & extension) :
                    value(value),
                    directoryName(directoryName),
                    fileName(fileName),
                    baseName(baseName),
                    number(number),
                    extension(extension)
                {}
                std::string value;
                std::string directoryName;
                std::string fileName;
                std::string baseName;
                std::string number;
                std::string extension;
            };
            const std::vector<Data> data =
            {
                Data("",                      "",          "",                 "",        "",      ""),
                Data("/",                     "/",         "",                 "",        "",      ""),
                Data("//",                    "//",        "",                 "",        "",      ""),
                Data(".",                     "",          ".",                ".",       "",      ""),
                Data("/.",                    "/",         ".",                ".",       "",      ""),
                Data("1.exr",                 "",          "1.exr",            "",        "1",     ".exr"),
                Data(".exr",                  "",          ".exr",             ".exr",    "",      ""),
                Data("/tmp",                  "/",         "tmp",              "tmp",     "",      ""),
                Data("//tmp",                 "//",        "tmp",              "tmp",     "",      ""),
                Data("/tmp/",                 "/tmp/",     "",                 "",        "",      ""),
                Data("//tmp/",                "//tmp/",    "",                 "",        "",      ""),
                Data("render.1.exr",          "",          "render.1.exr",     "render.", "1",     ".exr"),
                Data("render.100.exr",        "",          "render.100.exr",   "render.", "100",   ".exr"),
                Data("render1.exr",           "",          "render1.exr",      "render",  "1",     ".exr"),
                Data("render100.exr",         "",          "render100.exr",    "render",  "100",   ".exr"),
                Data("render-1.exr",          "",          "render-1.exr",     "render",  "-1",    ".exr"),
                Data("render-100.exr",        "",          "render-100.exr",   "render",  "-100",  ".exr"),
                Data("render####.exr",        "",          "render####.exr",   "render",  "####",  ".exr"),
                Data("1.exr",                 "",          "1.exr",            "",        "1",     ".exr"),
                Data("100.exr",               "",          "100.exr",          "",        "100",   ".exr"),
                Data("1",                     "",          "1",                "",        "1",     ""),
                Data("100",                   "",          "100",              "",        "100",   ""),
                Data("/tmp/render.1.exr",     "/tmp/",     "render.1.exr",     "render.", "1",     ".exr"),
                Data("C:\\",                  "C:\\",      "",                 "",        "",      ""),
                Data("C:\\tmp\\render.1.exr", "C:\\tmp\\", "render.1.exr",     "render.", "1",     ".exr"),
                Data("C:/tmp/render.1.exr",   "C:/tmp/",   "render.1.exr",     "render.", "1",     ".exr"),
                Data("tmp/render.1.exr",      "tmp/",      "render.1.exr",     "render.", "1",     ".exr")
            };
            for (const auto & d : data)
            {
                const File::Path value(d.value);
                const std::string & directoryName = value.getDirectoryName();
                const std::string & fileName = value.getFileName();
                const std::string & baseName = value.getBaseName();
                const std::string & number = value.getNumber();
                const std::string & extension = value.getExtension();
                std::stringstream ss;
                ss << "Components: " << d.value << " = " << directoryName << "|" << fileName << "|" << baseName << "|" << number << "|" << extension;
                _print(ss.str());
                DJV_ASSERT(directoryName == d.directoryName);
                DJV_ASSERT(fileName == d.fileName);
                DJV_ASSERT(baseName == d.baseName);
                DJV_ASSERT(number == d.number);
                DJV_ASSERT(extension == d.extension);
                DJV_ASSERT(d.value == directoryName + baseName + number + extension);
            }
        }
        
        void PathTest::_operators()
        {
            {
                const File::Path path("/a/b");
                DJV_ASSERT(path == path);
                File::Path path2;
                DJV_ASSERT(path2 != path);
                DJV_ASSERT(path2 < path);
            }

            {
                const File::Path path("/a/b");
                const File::Path path2("/a/b/");
                DJV_ASSERT(path == path2);
            }
        }

        void PathTest::_enum()
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

        void PathTest::_util()
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
                
        void PathTest::_serialize()
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

