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

#include <djvCoreTest/FileInfoTest.h>

#include <djvCore/FileIO.h>
#include <djvCore/FileInfo.h>

namespace djv
{
    using namespace Core;

    namespace CoreTest
    {
        FileInfoTest::FileInfoTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::FileInfoTest", context),
            _fileName("FileInfoTest"),
            _sequenceName("/tmp/render.1-100.exr")
        {}
        
        void FileInfoTest::run(const std::vector<std::string>& args)
        {
            _enum();
            _ctor();
            _path();
            _sequences();
            _util();
            _operators();
            _serialize();
        }

        void FileInfoTest::_enum()
        {
            for (auto i : FileSystem::getFileTypeEnums())
            {
                std::stringstream ss;
                ss << "File type string: " << i;
                _print(ss.str());
            }
            for (auto i :
                {
                    FileSystem::FilePermissions::Read,
                    FileSystem::FilePermissions::Write,
                    FileSystem::FilePermissions::Exec
                })
            {
                std::stringstream ss;
                ss << "File permissions string: " << FileSystem::getFilePermissionsLabel(static_cast<int>(i));
                _print(ss.str());
            }
            for (auto i : FileSystem::getDirectoryListSortEnums())
            {
                std::stringstream ss;
                ss << "Directory list sort string: " << i;
                _print(ss.str());
            }
        }

        void FileInfoTest::_ctor()
        {
            {
                const FileSystem::FileInfo fileInfo;
                DJV_ASSERT(fileInfo.isEmpty());
            }
            {
                const FileSystem::Path path("/");
                const FileSystem::FileInfo fileInfo(path, false);
                DJV_ASSERT(path == fileInfo.getPath());
                DJV_ASSERT(!fileInfo.isEmpty());
            }
            {
                const FileSystem::Path path("/");
                const FileSystem::FileType fileType = FileSystem::FileType::File;
                const FileSystem::FileInfo fileInfo(path, fileType, false);
                DJV_ASSERT(fileType == fileInfo.getType());
            }
            {
                const std::string path = "/";
                const FileSystem::FileInfo fileInfo(path, false);
                DJV_ASSERT(path == fileInfo.getPath().get());
            }
        }

        void FileInfoTest::_path()
        {
            {
                FileSystem::FileIO io;
                io.open(_fileName, FileSystem::FileIO::Mode::Write);
                io.close();
            }
            {
                FileSystem::FileInfo fileInfo;
                fileInfo.setPath(FileSystem::Path(_fileName));
                DJV_ASSERT(fileInfo.doesExist());
                DJV_ASSERT(FileSystem::FileType::File == fileInfo.getType());
                {
                    std::stringstream ss;
                    ss << "Size: " << fileInfo.getSize();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "User: " << fileInfo.getUser();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Permissions: " << fileInfo.getPermissions();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Time: " << fileInfo.getTime();
                    _print(ss.str());
                }
            }
            {
                FileSystem::FileInfo fileInfo;
                fileInfo.setPath(FileSystem::Path(_fileName), false);
                fileInfo.stat();
                DJV_ASSERT(FileSystem::FileType::File == fileInfo.getType());
                {
                    std::stringstream ss;
                    ss << "Size: " << fileInfo.getSize();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "User: " << fileInfo.getUser();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Permissions: " << fileInfo.getPermissions();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Time: " << fileInfo.getTime();
                    _print(ss.str());
                }
            }
            {
                FileSystem::FileInfo fileInfo(_sequenceName);
                DJV_ASSERT(_sequenceName == fileInfo.getFileName());
                DJV_ASSERT("/tmp/render.1.exr" != fileInfo.getFileName(1));
                fileInfo.evalSequence();
                DJV_ASSERT("/tmp/render.1.exr" == fileInfo.getFileName(1));
                DJV_ASSERT("render.1.exr" == fileInfo.getFileName(1, false));
            }
        }

        void FileInfoTest::_sequences()
        {
            {
                FileSystem::FileInfo fileInfo(_sequenceName);
                DJV_ASSERT(!fileInfo.isSequenceValid());
                DJV_ASSERT(!fileInfo.isSequenceWildcard());
                DJV_ASSERT(!fileInfo.isCompatible(fileInfo));
                fileInfo.evalSequence();
                Frame::Range range(1, 100);
                fileInfo.setSequence(range);
                DJV_ASSERT(Frame::Sequence(range) == fileInfo.getSequence());
                DJV_ASSERT(fileInfo.isSequenceValid());
                FileSystem::FileInfo fileInfo2(fileInfo.getFileName(101));
                fileInfo2.evalSequence();
                DJV_ASSERT(fileInfo.isCompatible(fileInfo2));
                fileInfo.addToSequence(fileInfo2);
                std::cout << fileInfo.getSequence() << std::endl;
                DJV_ASSERT(Frame::Sequence(Frame::Range(1, 101)) == fileInfo.getSequence());
            }
        }

        void FileInfoTest::_util()
        {
            for (const auto& i : FileSystem::FileInfo::directoryList(FileSystem::Path(".")))
            {
                _print(i.getFileName(Frame::invalid, false));
            }
            {
                const FileSystem::Path path = FileSystem::Path(".");
                const std::vector<std::string> fileNames =
                {
                    "render.1.exr",
                    "render.2.exr",
                    "render.3.exr"
                };
                for (const auto& i : fileNames)
                {
                    FileSystem::FileIO io;
                    io.open(i, FileSystem::FileIO::Mode::Write);
                }
                const FileSystem::FileInfo fileInfo = FileSystem::FileInfo::getFileSequence(
                    FileSystem::Path(".", fileNames[0]),
                    { ".exr" });
                std::stringstream ss;
                ss << "File sequence: " << fileInfo;
                _print(ss.str());
                DJV_ASSERT(fileInfo.getFileName(Frame::invalid, false) == "render.1-3.exr");
            }
        }

        void FileInfoTest::_operators()
        {
            {
                const FileSystem::FileInfo fileInfo(_fileName);
                DJV_ASSERT(fileInfo == fileInfo);
                DJV_ASSERT(fileInfo != FileSystem::FileInfo());
                DJV_ASSERT(fileInfo < FileSystem::FileInfo());
            }
        }

        void FileInfoTest::_serialize()
        {
            for (const auto i : FileSystem::getFileTypeEnums())
            {
                auto json = toJSON(i);
                FileSystem::FileType j = FileSystem::FileType::First;
                fromJSON(json, j);
                DJV_ASSERT(i == j);
            }
            for (const auto i : FileSystem::getDirectoryListSortEnums())
            {
                auto json = toJSON(i);
                FileSystem::DirectoryListSort j = FileSystem::DirectoryListSort::First;
                fromJSON(json, j);
                DJV_ASSERT(i == j);
            }
            {
                const FileSystem::FileInfo fileInfo(_fileName);
                auto json = toJSON(fileInfo);
                FileSystem::FileInfo fileInfo2;
                fromJSON(json, fileInfo2);
                DJV_ASSERT(fileInfo == fileInfo2);
            }
        }
        
    } // namespace CoreTest
} // namespace djv

