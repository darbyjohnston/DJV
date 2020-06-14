// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/FileInfoTest.h>

#include <djvCore/FileIO.h>
#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        FileInfoTest::FileInfoTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::FileInfoTest", context),
            _fileName("FileInfoTest"),
            _sequenceName("/tmp/render.1-100.exr")
        {}
        
        void FileInfoTest::run()
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
                ss << i;
                std::stringstream ss2;
                ss2 << "file type string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i :
                {
                    FileSystem::FilePermissions::Read,
                    FileSystem::FilePermissions::Write,
                    FileSystem::FilePermissions::Exec
                })
            {
                std::stringstream ss;
                ss << "file permissions string: " << FileSystem::getFilePermissionsLabel(static_cast<int>(i));
                _print(ss.str());
            }
            
            for (auto i : FileSystem::getDirectoryListSortEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "directory list sort string: " << _getText(ss.str());
                _print(ss2.str());
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
                const Frame::Sequence sequence(1, 10);
                const FileSystem::FileInfo fileInfo(path, fileType, sequence, false);
                DJV_ASSERT(fileType == fileInfo.getType());
                DJV_ASSERT(sequence == fileInfo.getSequence());
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
                auto io = FileSystem::FileIO::create();
                io->open(_fileName, FileSystem::FileIO::Mode::Write);
            }
            
            {
                FileSystem::FileInfo fileInfo;
                fileInfo.setPath(FileSystem::Path(_fileName));
                DJV_ASSERT(fileInfo.doesExist());
                DJV_ASSERT(FileSystem::FileType::File == fileInfo.getType());
                {
                    std::stringstream ss;
                    ss << "size: " << fileInfo.getSize();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "user: " << fileInfo.getUser();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "permissions: " << fileInfo.getPermissions();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "time: " << fileInfo.getTime();
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
                    ss << "size: " << fileInfo.getSize();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "user: " << fileInfo.getUser();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "permissions: " << fileInfo.getPermissions();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "time: " << fileInfo.getTime();
                    _print(ss.str());
                }
            }
            
            {
                FileSystem::FileInfo fileInfo("/tmp/render.1.exr");
                fileInfo.addToSequence(FileSystem::FileInfo("/tmp/render.3.exr"));
                fileInfo.addToSequence(FileSystem::FileInfo("/tmp/render.2.exr"));
                fileInfo.addToSequence(FileSystem::FileInfo("/tmp/render.4.exr"));
                DJV_ASSERT("/tmp/render.1-4.exr" == fileInfo.getFileName());
                DJV_ASSERT("/tmp/render.1.exr" == fileInfo.getFileName(1));
                DJV_ASSERT("render.1.exr" == fileInfo.getFileName(1, false));
            }
            
            {
                const std::string root(1, FileSystem::Path::getCurrentSeparator());
                FileSystem::FileInfo fileInfo(root);
                DJV_ASSERT(root == fileInfo.getFileName());
            }
        }

        void FileInfoTest::_sequences()
        {
            {
                FileSystem::FileInfo fileInfo(_sequenceName);
                DJV_ASSERT(Frame::Sequence() == fileInfo.getSequence());
                Frame::Sequence sequence(1, 100);
                fileInfo.setSequence(sequence);
                DJV_ASSERT(sequence == fileInfo.getSequence());
                FileSystem::FileInfo fileInfo2(fileInfo);
                fileInfo2.setSequence(Frame::Sequence(Frame::Range(101, 110), 4));
                DJV_ASSERT(fileInfo.isCompatible(fileInfo2));
                fileInfo.addToSequence(fileInfo2);
                DJV_ASSERT(Frame::Sequence(Frame::Range(1, 110), 4) == fileInfo.getSequence());
            }
            
            {
                FileSystem::FileInfo fileInfo("render.1.exr");
                DJV_ASSERT(!fileInfo.isCompatible(FileSystem::FileInfo("render.1.png")));
            }
            
            {
                FileSystem::FileInfo fileInfo("render.1.exr");
                DJV_ASSERT(!fileInfo.isCompatible(FileSystem::FileInfo("snapshot.1.exr")));
            }
        }

        void FileInfoTest::_util()
        {
            {
                DJV_ASSERT(!FileSystem::FileInfo::isSequenceWildcard(std::string()));
                DJV_ASSERT(FileSystem::FileInfo::isSequenceWildcard("#"));
                DJV_ASSERT(FileSystem::FileInfo::isSequenceWildcard("####"));
                DJV_ASSERT(!FileSystem::FileInfo::isSequenceWildcard("#0"));
            }
            
            {
                std::vector<FileSystem::DirectoryListOptions> optionsList;
                for (auto i : FileSystem::getDirectoryListSortEnums())
                {
                    FileSystem::DirectoryListOptions options;
                    options.sort = i;
                    optionsList.push_back(options);
                }
                for (const auto& i : optionsList)
                {
                    FileSystem::FileInfo::directoryList(FileSystem::Path("."), i);
                }
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
                    auto io = FileSystem::FileIO::create();
                    io->open(i, FileSystem::FileIO::Mode::Write);
                }
                const FileSystem::FileInfo fileInfo = FileSystem::FileInfo::getFileSequence(
                    FileSystem::Path(".", fileNames[0]),
                    { ".exr" });
                std::stringstream ss;
                ss << "file sequence: " << fileInfo;
                _print(ss.str());
                DJV_ASSERT(fileInfo.getFileName(Frame::invalid, false) == "render.1-3.exr");
            }
            
            {
                FileSystem::Path path;
                const FileSystem::FileInfo fileInfo = FileSystem::FileInfo::getFileSequence(path, {});
                DJV_ASSERT(fileInfo.getPath() == path);
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
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(i, allocator);
                FileSystem::FileType j = FileSystem::FileType::First;
                fromJSON(json, j);
                DJV_ASSERT(i == j);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                FileSystem::FileType t;
                fromJSON(json, t);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            for (const auto i : FileSystem::getDirectoryListSortEnums())
            {
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(i, allocator);
                FileSystem::DirectoryListSort j = FileSystem::DirectoryListSort::First;
                fromJSON(json, j);
                DJV_ASSERT(i == j);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                FileSystem::DirectoryListSort t;
                fromJSON(json, t);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const FileSystem::FileInfo fileInfo(_fileName);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(fileInfo, allocator);
                FileSystem::FileInfo fileInfo2;
                fromJSON(json, fileInfo2);
                DJV_ASSERT(fileInfo == fileInfo2);
            }

            try
            {
                auto json = rapidjson::Value();
                FileSystem::FileInfo fileInfo;
                fromJSON(json, fileInfo);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace CoreTest
} // namespace djv

