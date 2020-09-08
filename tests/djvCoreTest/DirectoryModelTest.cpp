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
        DirectoryModelTest::DirectoryModelTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITickTest(
                "djv::CoreTest::DirectoryModelTest",
                FileSystem::Path(tempPath, "DirectoryModelTest"),
                context)
        {}
        
        void DirectoryModelTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto model = FileSystem::DirectoryModel::create(context);
                
                FileSystem::Path path;
                std::vector<FileSystem::FileInfo> fileInfo;
                std::vector<std::string> fileNames;
                bool hasUp = false;
                std::vector<FileSystem::Path> history;
                size_t historyIndex = 0;
                bool hasBack = false;
                bool hasForward = false;
                FileSystem::DirectoryListOptions options;
                
                auto pathObserver = ValueObserver<FileSystem::Path>::create(
                    model->observePath(),
                    [&path](const FileSystem::Path& value)
                    {
                        path = value;
                    });
                auto fileInfoObserver = ListObserver<FileSystem::FileInfo>::create(
                    model->observeFileInfo(),
                    [&fileInfo](const std::vector<FileSystem::FileInfo>& value)
                    {
                        fileInfo = value;
                    });
                auto fileNamesObserver = ListObserver<std::string>::create(
                    model->observeFileNames(),
                    [&fileNames](const std::vector<std::string>& value)
                    {
                        fileNames = value;
                    });
                auto hasUpObserver = ValueObserver<bool>::create(
                    model->observeHasUp(),
                    [&hasUp](bool value)
                    {
                        hasUp = value;
                    });
                auto historyObserver = ListObserver<FileSystem::Path>::create(
                    model->observeHistory(),
                    [&history](const std::vector<FileSystem::Path>& value)
                    {
                        history = value;
                    });
                auto historyIndexObserver = ValueObserver<size_t>::create(
                    model->observeHistoryIndex(),
                    [&historyIndex](size_t value)
                    {
                        historyIndex = value;
                    });
                auto hasBackObserver = ValueObserver<bool>::create(
                    model->observeHasBack(),
                    [&hasBack](bool value)
                    {
                        hasBack = value;
                    });
                auto hasForwardObserver = ValueObserver<bool>::create(
                    model->observeHasForward(),
                    [&hasForward](bool value)
                    {
                        hasForward = value;
                    });
                auto optionsObserver = ValueObserver<FileSystem::DirectoryListOptions>::create(
                    model->observeOptions(),
                    [&options](const FileSystem::DirectoryListOptions& value)
                    {
                        options = value;
                    });
                
                model->setPath(getTempPath());
                {
                    _print("Path: " + path.get());
                }
                model->reload();
                for (const auto& i : fileInfo)
                {
                    _print("File info: " + std::string(i));
                }
                for (const auto& i : fileNames)
                {
                    _print("File name: " + i);
                }

                const auto pathA = path;
                model->cdUp();
                const auto pathB = path;
                {
                    _print("Path: " + path.get());
                }
                DJV_ASSERT(hasUp);

                model->setHistoryMax(100);
                model->setHistoryMax(100);
                model->setHistoryMax(10);
                model->setHistoryIndex(0);
                DJV_ASSERT(path == pathA);
                model->goForward();
                DJV_ASSERT(path == pathB);
                model->goBack();
                DJV_ASSERT(path == pathA);
                for (const auto& i : history)
                {
                    _print("History: " + i.get());
                }
                DJV_ASSERT(0 == historyIndex);
                DJV_ASSERT(!hasBack);
                DJV_ASSERT(hasForward);
                model->setPath(pathB);
                model->setPath(pathA);
                model->setHistoryMax(0);

                FileSystem::DirectoryListOptions optionsA;
                optionsA.extensions.insert(".txt");
                optionsA.sequences = true;
                optionsA.sequenceExtensions.insert(".txt");
                optionsA.showHidden = true;
                optionsA.sort = FileSystem::DirectoryListSort::Size;
                optionsA.reverseSort = true;
                optionsA.sortDirectoriesFirst = false;
                optionsA.filter = ".txt";
                model->setOptions(optionsA);
                DJV_ASSERT(options == optionsA);
                
                _tickFor(std::chrono::milliseconds(1000));

                auto io = FileSystem::FileIO::create();
                io->open(
                    FileSystem::Path(path, "file.txt").get(),
                    FileSystem::FileIO::Mode::Write);
                io->close();

                _tickFor(std::chrono::milliseconds(1000));
            }
        }
        
    } // namespace CoreTest
} // namespace djv

