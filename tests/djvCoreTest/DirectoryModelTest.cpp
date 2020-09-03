// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/DirectoryModelTest.h>

#include <djvCore/DirectoryModel.h>
#include <djvCore/FileIO.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        DirectoryModelTest::DirectoryModelTest(const std::shared_ptr<Core::Context>& context) :
            ITickTest("djv::CoreTest::DirectoryModelTest", context)
        {}
        
        void DirectoryModelTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto model = FileSystem::DirectoryModel::create(context);
                model->setPath(FileSystem::Path("."));
                auto path = model->observePath()->get();
                {
                    std::stringstream ss;
                    ss << "path: " << path;
                    _print(ss.str());
                }
                model->reload();
                for (const auto& i : model->observeFileInfo()->get())
                {
                    std::stringstream ss;
                    ss << "file info: " << i;
                    _print(ss.str());
                }
                for (const auto& i : model->observeFileNames()->get())
                {
                    std::stringstream ss;
                    ss << "file name: " << i;
                    _print(ss.str());
                }

                model->cdUp();
                auto path2 = model->observePath()->get();
                {
                    std::stringstream ss;
                    ss << "path 2: " << path2;
                    _print(ss.str());
                }
                DJV_ASSERT(model->observeHasUp()->get());

                model->setHistoryMax(100);
                model->setHistoryMax(100);
                model->setHistoryMax(10);
                model->setHistoryIndex(0);
                DJV_ASSERT(path == model->observePath()->get());
                model->goForward();
                DJV_ASSERT(path2 == model->observePath()->get());
                model->goBack();
                DJV_ASSERT(path == model->observePath()->get());
                for (const auto& i : model->observeHistory()->get())
                {
                    std::stringstream ss;
                    ss << "history: " << i;
                    _print(ss.str());
                }
                DJV_ASSERT(0 == model->observeHistoryIndex()->get());
                DJV_ASSERT(!model->observeHasBack()->get());
                DJV_ASSERT(model->observeHasForward()->get());
                model->setPath(path2);
                model->setPath(path);
                model->setHistoryMax(0);

                FileSystem::DirectoryListOptions options;
                options.extensions.insert(".txt");
                options.sequences = true;
                options.sequenceExtensions.insert(".txt");
                options.showHidden = true;
                options.sort = FileSystem::DirectoryListSort::Size;
                options.reverseSort = true;
                options.sortDirectoriesFirst = false;
                options.filter = ".txt";
                model->setOptions(options);
                DJV_ASSERT(options == model->observeOptions()->get());
                
                _tickFor(std::chrono::milliseconds(1000));

                auto io = FileSystem::FileIO::create();
                io->open("DirectoryModelTest", FileSystem::FileIO::Mode::Write);
                io->close();

                _tickFor(std::chrono::milliseconds(1000));
            }
        }
        
    } // namespace CoreTest
} // namespace djv

