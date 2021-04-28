// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/FileInfoTest.h>

#include <djvSystem/FileIO.h>
#include <djvSystem/FileInfo.h>

#include <djvMath/FrameNumber.h>

#include <iomanip>
#include <sstream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        FileInfoTest::FileInfoTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest(
                "djv::SystemTest::FileInfoTest",
                File::Path(tempPath, "FileInfoTest"),
                context),
            _fileName("file.txt"),
            _sequenceName("render.#.exr"),
            _sequence(1, 100)
        {}
        
        void FileInfoTest::run()
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
            
            _ctor();
            _path();
            _sequences();
            _operators();
            _enum();
            _util();
            _serialize();
        }

        void FileInfoTest::_ctor()
        {
            {
                const File::Info info;
                DJV_ASSERT(info.isEmpty());
            }
            
            {
                const File::Path path("/");
                const File::Info info(path, false);
                DJV_ASSERT(path == info.getPath());
                DJV_ASSERT(!info.isEmpty());
            }
            
            {
                const File::Path path("/");
                const File::Type type = File::Type::File;
                const Math::Frame::Sequence sequence(1, 10);
                const File::Info info(path, type, sequence, false);
                DJV_ASSERT(type == info.getType());
                DJV_ASSERT(sequence == info.getSequence());
            }
            
            {
                const std::string path = "/";
                const File::Info info(path, false);
                DJV_ASSERT(path == info.getPath().get());
            }
        }

        void FileInfoTest::_path()
        {
            {
                File::Info info;
                info.setPath(File::Path(getTempPath(), _fileName));
                DJV_ASSERT(info.doesExist());
                DJV_ASSERT(File::Type::File == info.getType());
                {
                    std::stringstream ss;
                    ss << "Size: " << info.getSize();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "User: " << info.getUser();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Permissions: " << info.getPermissions();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Time: " << info.getTime();
                    _print(ss.str());
                }
            }
            
            {
                File::Info info;
                info.setPath(File::Path(getTempPath(), _fileName), false);
                DJV_ASSERT(info.stat());
                DJV_ASSERT(File::Type::File == info.getType());
                {
                    std::stringstream ss;
                    ss << "Size: " << info.getSize();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "User: " << info.getUser();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Permissions: " << info.getPermissions();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Time: " << info.getTime();
                    _print(ss.str());
                }
            }
            
            {
                File::Info info(
                    File::Path(getTempPath(), _sequenceName),
                    File::Type::Sequence,
                    _sequence,
                    false);
                DJV_ASSERT(info.stat());
                DJV_ASSERT(File::Type::Sequence == info.getType());
                {
                    std::stringstream ss;
                    ss << "Size: " << info.getSize();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "User: " << info.getUser();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Permissions: " << info.getPermissions();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Time: " << info.getTime();
                    _print(ss.str());
                }
            }
            
            {
                File::Info info("/tmp/render.1.exr");
                info.addToSequence(File::Info("/tmp/render.3.exr"));
                info.addToSequence(File::Info("/tmp/render.2.exr"));
                info.addToSequence(File::Info("/tmp/render.4.exr"));
                DJV_ASSERT("/tmp/render.1-4.exr" == info.getFileName());
                DJV_ASSERT("/tmp/render.1.exr" == info.getFileName(1));
                DJV_ASSERT("render.1.exr" == info.getFileName(1, false));
            }
            
            {
                const std::string root(1, File::Path::getCurrentSeparator());
                File::Info info(root);
                DJV_ASSERT(root == info.getFileName());
            }
        }

        void FileInfoTest::_sequences()
        {
            {
                File::Info info(_sequenceName);
                DJV_ASSERT(Math::Frame::Sequence() == info.getSequence());
                info.setSequence(_sequence);
                DJV_ASSERT(_sequence == info.getSequence());
                File::Info info2(info);
                info2.setSequence(Math::Frame::Sequence(Math::Frame::Range(101, 110), 4));
                DJV_ASSERT(info.isCompatible(info2));
                info.addToSequence(info2);
                DJV_ASSERT(Math::Frame::Sequence(Math::Frame::Range(1, 110), 4) == info.getSequence());
            }
            
            {
                File::Info info("render.1.exr");
                DJV_ASSERT(!info.isCompatible(File::Info("render.1.png")));
            }
            
            {
                File::Info info("render.1.exr");
                DJV_ASSERT(!info.isCompatible(File::Info("snapshot.1.exr")));
            }

            {
                File::Info info("frame#.png");
                info.setSequence(Math::Frame::Sequence(10));
                info.addToSequence(File::Info("frame09.png"));
                DJV_ASSERT(2 == info.getSequence().getPad());
            }

            {
                File::Info info("render.1.exr");
                DJV_ASSERT(!info.isCompatible(File::Info("/tmp/render.1.exr")));
            }
        }

        void FileInfoTest::_operators()
        {
            {
                const File::Info info(_fileName);
                DJV_ASSERT(info == info);
                DJV_ASSERT(info != File::Info());
                DJV_ASSERT(info < File::Info());
                DJV_ASSERT(_fileName == std::string(info));
            }
        }
        
        void FileInfoTest::_enum()
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

        void FileInfoTest::_util()
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

        void FileInfoTest::_serialize()
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

