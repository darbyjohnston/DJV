// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/PathTest.h>

#include <djvCore/Error.h>
#include <djvCore/Path.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        PathTest::PathTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::PathTest", context)
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
                std::stringstream ss2;
                ss2 << "resource path string: " << _getText(ss.str());
                _print(ss2.str());
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
                DJV_ASSERT(!path.isRoot());
                DJV_ASSERT(path.cdUp('/'));
                DJV_ASSERT("/a" == path.get());
                DJV_ASSERT(path.cdUp('/'));
                DJV_ASSERT("/" == path.get());
                DJV_ASSERT(!path.cdUp('/'));
                path.append("a", '/');
                path.append("b", '/');
                DJV_ASSERT("/a/b" == path.get());
                path.set("/");
                DJV_ASSERT(path.isRoot());
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
                Data("render.1,2,3.exr",      "",          "render.1,2,3.exr", "render.", "1,2,3", ".exr"),
                Data("render1.exr",           "",          "render1.exr",      "render",  "1",     ".exr"),
                Data("render100.exr",         "",          "render100.exr",    "render",  "100",   ".exr"),
                Data("render-1.exr",          "",          "render-1.exr",     "render-", "1",     ".exr"),
                Data("render-100.exr",        "",          "render-100.exr",   "render-", "100",   ".exr"),
                Data("render1-100.exr",       "",          "render1-100.exr",  "render",  "1-100", ".exr"),
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
                ss << "components: " << d.value << " = " << directoryName << "|" << fileName << "|" << baseName << "|" << number << "|" << extension;
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
                DJV_ASSERT(FileSystem::Path::isSeparator('/'));
                DJV_ASSERT('/' == FileSystem::Path::getSeparator(FileSystem::PathSeparator::Unix));
                {
                    std::stringstream ss;
                    ss << "current separator: " << FileSystem::Path::getCurrentSeparator();
                    _print(ss.str());
                }
            }
            
            {
                std::string path("/a/b/");
                FileSystem::Path::removeTrailingSeparator(path);
                DJV_ASSERT("/a/b" == path);
            }
            
            {
                struct Data
                {
                    std::string path;
                    char        seperator;
                };
                std::vector<Data> data =
                {
                    { "a", '/' },
                    { "a/b", '/' },
                    { "/a/b", '/' },
                    { "/", '/' },
                    { "a\\b", '\\' },
                    { "\\a\\b", '\\' },
                    { "a:\\b", '\\' },
                    { "\\", '\\' },
                    { "\\\\a", '\\' },
                    { "\\\\a\\b", '\\' }
                };
                for (const auto & d : data)
                {
                    const auto pieces = FileSystem::Path::splitDir(d.path);
                    const std::string path = FileSystem::Path::joinDirs(pieces, d.seperator);
                    std::stringstream ss;
                    ss << "split/join: " << d.path << " = " << path;
                    _print(ss.str());
                    DJV_ASSERT(d.path == path);
                }
            }
            
            {
                const FileSystem::Path path("PathTestMkdir");
                FileSystem::Path::mkdir(path);
                FileSystem::Path::rmdir(path);
            }

            {            
                const FileSystem::Path path("PathTestMkdir");
                try
                {
                    FileSystem::Path::mkdir(path);
                    FileSystem::Path::mkdir(path);
                }
                catch (const std::exception & e)
                {
                    _print(Error::format(e));
                }
                try
                {
                    FileSystem::Path::rmdir(path);
                    FileSystem::Path::rmdir(path);
                }
                catch (const std::exception & e)
                {
                    _print(Error::format(e));
                }
            }
            
            {
                const FileSystem::Path path = FileSystem::Path::getAbsolute(FileSystem::Path("."));
                std::stringstream ss;
                ss << "absolute: " << path;
                _print(ss.str());
            }
            
            {
                const FileSystem::Path path = FileSystem::Path::getAbsolute(FileSystem::Path());
                std::stringstream ss;
                ss << "absolute: " << path;
                _print(ss.str());
            }

            {
                const FileSystem::Path path = FileSystem::Path::getCWD();
                std::stringstream ss;
                ss << "cwd: " << path;
                _print(ss.str());
            }

            {
                const FileSystem::Path path = FileSystem::Path::getTemp();
                std::stringstream ss;
                ss << "temp: " << path;
                _print(ss.str());
            }
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
                auto json = toJSON(path);
                FileSystem::Path path2;
                fromJSON(json, path2);
                DJV_ASSERT(path == path2);
            }

            try            
            {
                auto json = picojson::value(picojson::object_type, true);
                FileSystem::Path path;
                fromJSON(json, path);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
                
    } // namespace CoreTest
} // namespace djv

