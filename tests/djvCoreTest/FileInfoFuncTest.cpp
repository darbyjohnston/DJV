// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/FileInfoFuncTest.h>

#include <djvCore/FileIO.h>
#include <djvCore/FileInfoFunc.h>
#include <djvCore/FrameNumberFunc.h>

#include <iomanip>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        FileInfoFuncTest::FileInfoFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest(
                "djv::CoreTest::FileInfoFuncTest",
                FileSystem::Path(tempPath, "FileInfoFuncTest"),
                context),
            _fileName("file.txt"),
            _sequenceName("render.#.exr"),
            _sequence(1, 100)
        {}
        
        void FileInfoFuncTest::run()
        {
            auto io = FileSystem::FileIO::create();
            io->open(
                FileSystem::Path(getTempPath(), _fileName).get(),
                FileSystem::FileIO::Mode::Write);
            
            const FileSystem::FileInfo fileInfo(FileSystem::Path(_sequenceName), FileSystem::FileType::Sequence, _sequence);
            for (const auto& i : Frame::toFrames(fileInfo.getSequence()))
            {
                io->open(
                    FileSystem::Path(getTempPath(), fileInfo.getFileName(i)).get(),
                    FileSystem::FileIO::Mode::Write);
            }
            
            _enum();
            _util();
            _serialize();
        }

        void FileInfoFuncTest::_enum()
        {
            for (auto i : FileSystem::getFileTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("File type: " + _getText(ss.str()));
            }
            
            for (auto i :
                {
                    FileSystem::FilePermissions::Read,
                    FileSystem::FilePermissions::Write,
                    FileSystem::FilePermissions::Exec
                })
            {
                std::stringstream ss;
                ss << "File permissions: " << FileSystem::getFilePermissionsLabel(static_cast<int>(i));
                _print(ss.str());
            }
            
            for (auto i : FileSystem::getDirectoryListSortEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Directory list sort: " + _getText(ss.str()));
            }
        }

        void FileInfoFuncTest::_util()
        {
            {
                DJV_ASSERT(!FileSystem::isSequenceWildcard(std::string()));
                DJV_ASSERT(FileSystem::isSequenceWildcard("#"));
                DJV_ASSERT(FileSystem::isSequenceWildcard("####"));
                DJV_ASSERT(!FileSystem::isSequenceWildcard("#0"));
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
                    FileSystem::directoryList(FileSystem::Path(getTempPath()), i);
                }
            }
            
            {
                FileSystem::DirectoryListOptions options;
                options.extensions.insert(".exr");
                FileSystem::directoryList(FileSystem::Path(getTempPath()), options);
            }
            
            {
                const FileSystem::FileInfo fileInfo = FileSystem::getFileSequence(
                    FileSystem::Path(getTempPath(), "render.1.exr"),
                    { ".exr" });
                std::stringstream ss;
                ss << "File sequence: " << fileInfo;
                _print(ss.str());
                DJV_ASSERT(fileInfo.getFileName(Frame::invalid, false) == "render.1-100.exr");
            }
            
            {
                FileSystem::Path path;
                const auto fileInfo = FileSystem::getFileSequence(path, {});
                DJV_ASSERT(fileInfo.getPath() == path);
            }
        }

        void FileInfoFuncTest::_serialize()
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
            
            {
                const FileSystem::FileInfo fileInfo(FileSystem::Path("render.1-100.exr"), FileSystem::FileType::Sequence, _sequence, false);
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

