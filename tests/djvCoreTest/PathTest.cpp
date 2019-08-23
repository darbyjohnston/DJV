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

#include <djvCoreTest/PathTest.h>

#include <djvCore/Error.h>
#include <djvCore/Path.h>

namespace djv
{
    using namespace Core;

    namespace CoreTest
    {
        PathTest::PathTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::PathTest", context)
        {}
        
        void PathTest::run(const std::vector<std::string>& args)
        {
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
                    std::stringstream s;
                    s << "Components: " << d.value << " = " << directoryName << "|" << fileName << "|" << baseName << "|" << number << "|" << extension;
                    _print(s.str());
                    DJV_ASSERT(directoryName == d.directoryName);
                    DJV_ASSERT(fileName == d.fileName);
                    DJV_ASSERT(baseName == d.baseName);
                    DJV_ASSERT(number == d.number);
                    DJV_ASSERT(extension == d.extension);
                    DJV_ASSERT(d.value == directoryName + baseName + number + extension);
                }
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
                    std::stringstream s;
                    s << "splitDir()/joinDirs(): " << d.path << " = " << path;
                    _print(s.str());
                    DJV_ASSERT(d.path == path);
                }
            }
            {
                try
                {
                    FileSystem::Path path = FileSystem::Path::getAbsolute(FileSystem::Path("."));
                    std::stringstream s;
                    s << "getAbsolute(): " << path;
                    _print(s.str());
                }
                catch (const std::exception & e)
                {
                    std::cerr << Error::format(e) << std::endl;
                }
            }
            {
                FileSystem::Path path = FileSystem::Path::getCWD();
                std::stringstream s;
                s << "getCWD(): " << path;
                _print(s.str());
            }
        }
        
    } // namespace CoreTest
} // namespace djv

