// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/PathTest.h>

#include <djvCore/Error.h>
#if defined(DJV_PLATFORM_MACOS) || defined(DJV_PLATFORM_LINUX)
#include <djvCore/OS.h>
#endif // DJV_PLATFORM_MACOS || DJV_PLATFORM_LINUX
#include <djvCore/Path.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        PathTest::PathTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::PathTest", tempPath, context)
        {}
        
        void PathTest::run()
        {
            _enum();
            _path();
            _split();
            _util();
            _operators();
            _serialize();
        }
                
        void PathTest::_enum()
        {
            for (auto i : FileSystem::getResourcePathEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Resource path: " + _getText(ss.str()));
            }
        }
               
        void PathTest::_path()
        {
            {
                const FileSystem::Path path;
                DJV_ASSERT(path.isEmpty());
            }

            {
                FileSystem::Path path("/a/b");
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
                DJV_ASSERT(!FileSystem::Path().isRoot());
                DJV_ASSERT(FileSystem::Path("/").isRoot());
                DJV_ASSERT(!FileSystem::Path("/tmp").isRoot());
                DJV_ASSERT(FileSystem::Path("C:\\").isRoot());
                DJV_ASSERT(!FileSystem::Path("C:\\tmp").isRoot());
            }

            {
                DJV_ASSERT(!FileSystem::Path().isServer());
                DJV_ASSERT(!FileSystem::Path("\\\\").isServer());
                DJV_ASSERT(FileSystem::Path("\\\\server").isServer());
                DJV_ASSERT(FileSystem::Path("\\\\server\\").isServer());
                DJV_ASSERT(!FileSystem::Path("\\\\server\\share").isServer());
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
                const FileSystem::Path value(d.value);
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
        
        void PathTest::_util()
        {
            {
                DJV_ASSERT(FileSystem::isSeparator('/'));
                DJV_ASSERT('/' == FileSystem::getSeparator(FileSystem::PathSeparator::Unix));
                {
                    std::stringstream ss;
                    ss << "Current separator: " << FileSystem::getCurrentSeparator();
                    _print(ss.str());
                }
            }
            
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
        
        void PathTest::_operators()
        {
            const FileSystem::Path path("/a/b");
            DJV_ASSERT(path == path);
            FileSystem::Path path2;
            DJV_ASSERT(path2 != path);
            DJV_ASSERT(path2 < path);
        }
        
        void PathTest::_serialize()
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

