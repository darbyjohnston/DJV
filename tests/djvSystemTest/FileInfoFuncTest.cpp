// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/FileInfoFuncTest.h>

#include <djvSystem/FileIO.h>
#include <djvSystem/FileInfoFunc.h>

#include <djvMath/FrameNumberFunc.h>

#include <iomanip>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        FileInfoFuncTest::FileInfoFuncTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest(
                "djv::SystemTest::FileInfoFuncTest",
                File::Path(tempPath, "FileInfoFuncTest"),
                context),
            _fileName("file.txt"),
            _sequenceName("render.#.exr"),
            _sequence(1, 100)
        {}
        
        void FileInfoFuncTest::run()
        {
            auto io = File::IO::create();
            io->open(
                File::Path(getTempPath(), _fileName).get(),
                File::Mode::Write);
            
            const File::Info info(File::Path(_sequenceName), File::Type::Sequence, _sequence);
            for (const auto& i : Math::Frame::toFrames(info.getSequence()))
            {
                io->open(
                    File::Path(getTempPath(), info.getFileName(i)).get(),
                    File::Mode::Write);
            }
            
            _enum();
            _util();
            _serialize();
        }

        void FileInfoFuncTest::_enum()
        {
            for (auto i : File::getTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("File type: " + _getText(ss.str()));
            }
            
            for (auto i :
                {
                    File::Permissions::Read,
                    File::Permissions::Write,
                    File::Permissions::Exec
                })
            {
                std::stringstream ss;
                ss << "File permissions: " << File::getPermissionsLabel(static_cast<int>(i));
                _print(ss.str());
            }
            
            for (auto i : File::getDirectoryListSortEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Directory list sort: " + _getText(ss.str()));
            }
        }

        void FileInfoFuncTest::_util()
        {
            {
                DJV_ASSERT(!File::isSequenceWildcard(std::string()));
                DJV_ASSERT(File::isSequenceWildcard("#"));
                DJV_ASSERT(File::isSequenceWildcard("####"));
                DJV_ASSERT(!File::isSequenceWildcard("#0"));
            }
            
            {
                std::vector<File::DirectoryListOptions> optionsList;
                for (auto i : File::getDirectoryListSortEnums())
                {
                    File::DirectoryListOptions options;
                    options.sort = i;
                    optionsList.push_back(options);
                }
                for (const auto& i : optionsList)
                {
                    File::directoryList(File::Path(getTempPath()), i);
                }
            }
            
            {
                File::DirectoryListOptions options;
                options.extensions.insert(".exr");
                File::directoryList(File::Path(getTempPath()), options);
            }
            
            {
                const File::Info info = File::getSequence(
                    File::Path(getTempPath(), "render.1.exr"),
                    { ".exr" });
                std::stringstream ss;
                ss << "File sequence: " << info;
                _print(ss.str());
                DJV_ASSERT(info.getFileName(Math::Frame::invalid, false) == "render.1-100.exr");
            }
            
            {
                File::Path path;
                const auto info = File::getSequence(path, {});
                DJV_ASSERT(info.getPath() == path);
            }
        }

        void FileInfoFuncTest::_serialize()
        {
            for (const auto i : File::getTypeEnums())
            {
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(i, allocator);
                File::Type j = File::Type::First;
                fromJSON(json, j);
                DJV_ASSERT(i == j);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                File::Type t;
                fromJSON(json, t);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            for (const auto i : File::getDirectoryListSortEnums())
            {
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(i, allocator);
                File::DirectoryListSort j = File::DirectoryListSort::First;
                fromJSON(json, j);
                DJV_ASSERT(i == j);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                File::DirectoryListSort t;
                fromJSON(json, t);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const File::Info info(_fileName);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(info, allocator);
                File::Info info2;
                fromJSON(json, info2);
                DJV_ASSERT(info == info2);
            }
            
            {
                const File::Info info(File::Path("render.1-100.exr"), File::Type::Sequence, _sequence, false);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(info, allocator);
                File::Info info2;
                fromJSON(json, info2);
                DJV_ASSERT(info == info2);
            }

            try
            {
                auto json = rapidjson::Value();
                File::Info info;
                fromJSON(json, info);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace SystemTest
} // namespace djv

