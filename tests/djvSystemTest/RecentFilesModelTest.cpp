// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/RecentFilesModelTest.h>

#include <djvSystem/FileInfo.h>
#include <djvSystem/RecentFilesModel.h>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        RecentFilesModelTest::RecentFilesModelTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::SystemTest::RecentFilesModelTest", tempPath, context)
        {}
        
        void RecentFilesModelTest::run()
        {
            {
                auto model = File::RecentFilesModel::create();
                
                std::vector<File::Info> files;
                auto filesObserver = ListObserver<File::Info>::create(
                    model->observeFiles(),
                    [&files](const std::vector<File::Info>& value)
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
                    File::Info("file1"),
                    File::Info("file2"),
                    File::Info("file3"),
                    File::Info("file4") });
                DJV_ASSERT(3 == files.size());
                _print(std::string(files[0]));
                _print(std::string(files[1]));
                _print(std::string(files[2]));
                DJV_ASSERT(File::Info("file1") == files[0]);
                DJV_ASSERT(File::Info("file2") == files[1]);
                DJV_ASSERT(File::Info("file3") == files[2]);
                
                model->addFile(File::Info("file4"));
                DJV_ASSERT(3 == files.size());
                model->addFile(File::Info("file1"));
                DJV_ASSERT(3 == files.size());
                _print(std::string(files[0]));
                _print(std::string(files[1]));
                _print(std::string(files[2]));
                DJV_ASSERT(File::Info("file1") == files[0]);
                DJV_ASSERT(File::Info("file4") == files[1]);
                DJV_ASSERT(File::Info("file2") == files[2]);
                
                model->setFilesMax(2);
                DJV_ASSERT(2 == files.size());
                _print(std::string(files[0]));
                _print(std::string(files[1]));
                DJV_ASSERT(File::Info("file1") == files[0]);
                DJV_ASSERT(File::Info("file4") == files[1]);
            }
        }
        
    } // namespace SystemTest
} // namespace djv

