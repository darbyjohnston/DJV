// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/RecentFilesModelTest.h>

#include <djvCore/FileInfo.h>
#include <djvCore/RecentFilesModel.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        RecentFilesModelTest::RecentFilesModelTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::RecentFilesModelTest", tempPath, context)
        {}
        
        void RecentFilesModelTest::run()
        {
            {
                auto model = FileSystem::RecentFilesModel::create();
                
                std::vector<FileSystem::FileInfo> files;
                auto filesObserver = ListObserver<FileSystem::FileInfo>::create(
                    model->observeFiles(),
                    [&files](const std::vector<FileSystem::FileInfo>& value)
                    {
                        files = value;
                    });
                
                size_t max = 0;
                auto maxObserver = ValueObserver<size_t>::create(
                    model->observeFilesMax(),
                    [&max](size_t value)
                    {
                        max = value;
                    });

                model->setFilesMax(3);
                model->setFilesMax(3);
                DJV_ASSERT(3 == max);
                
                model->setFiles({
                    FileSystem::FileInfo("file1"),
                    FileSystem::FileInfo("file2"),
                    FileSystem::FileInfo("file3"),
                    FileSystem::FileInfo("file4") });
                DJV_ASSERT(3 == files.size());
                _print(std::string(files[0]));
                _print(std::string(files[1]));
                _print(std::string(files[2]));
                DJV_ASSERT(FileSystem::FileInfo("file1") == files[0]);
                DJV_ASSERT(FileSystem::FileInfo("file2") == files[1]);
                DJV_ASSERT(FileSystem::FileInfo("file3") == files[2]);
                
                model->addFile(FileSystem::FileInfo("file4"));
                DJV_ASSERT(3 == files.size());
                model->addFile(FileSystem::FileInfo("file1"));
                DJV_ASSERT(3 == files.size());
                _print(std::string(files[0]));
                _print(std::string(files[1]));
                _print(std::string(files[2]));
                DJV_ASSERT(FileSystem::FileInfo("file1") == files[0]);
                DJV_ASSERT(FileSystem::FileInfo("file4") == files[1]);
                DJV_ASSERT(FileSystem::FileInfo("file2") == files[2]);
                
                model->setFilesMax(2);
                DJV_ASSERT(2 == files.size());
                _print(std::string(files[0]));
                _print(std::string(files[1]));
                DJV_ASSERT(FileSystem::FileInfo("file1") == files[0]);
                DJV_ASSERT(FileSystem::FileInfo("file4") == files[1]);
            }
        }
        
    } // namespace CoreTest
} // namespace djv

