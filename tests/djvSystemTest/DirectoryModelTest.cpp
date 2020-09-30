// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/DirectoryModelTest.h>

#include <djvSystem/DirectoryModel.h>
#include <djvSystem/FileIO.h>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        DirectoryModelTest::DirectoryModelTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITickTest(
                "djv::SystemTest::DirectoryModelTest",
                File::Path(tempPath, "DirectoryModelTest"),
                context)
        {}
        
        void DirectoryModelTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto model = File::DirectoryModel::create(context);
                
                File::Path path;
                std::vector<File::Info> info;
                std::vector<std::string> fileNames;
                bool hasUp = false;
                std::vector<File::Path> history;
                size_t historyIndex = 0;
                bool hasBack = false;
                bool hasForward = false;
                File::DirectoryListOptions options;
                
                auto pathObserver = Observer::Value<File::Path>::create(
                    model->observePath(),
                    [&path](const File::Path& value)
                    {
                        path = value;
                    });
                auto infoObserver = Observer::List<File::Info>::create(
                    model->observeInfo(),
                    [&info](const std::vector<File::Info>& value)
                    {
                        info = value;
                    });
                auto fileNamesObserver = Observer::List<std::string>::create(
                    model->observeFileNames(),
                    [&fileNames](const std::vector<std::string>& value)
                    {
                        fileNames = value;
                    });
                auto hasUpObserver = Observer::Value<bool>::create(
                    model->observeHasUp(),
                    [&hasUp](bool value)
                    {
                        hasUp = value;
                    });
                auto historyObserver = Observer::List<File::Path>::create(
                    model->observeHistory(),
                    [&history](const std::vector<File::Path>& value)
                    {
                        history = value;
                    });
                auto historyIndexObserver = Observer::Value<size_t>::create(
                    model->observeHistoryIndex(),
                    [&historyIndex](size_t value)
                    {
                        historyIndex = value;
                    });
                auto hasBackObserver = Observer::Value<bool>::create(
                    model->observeHasBack(),
                    [&hasBack](bool value)
                    {
                        hasBack = value;
                    });
                auto hasForwardObserver = Observer::Value<bool>::create(
                    model->observeHasForward(),
                    [&hasForward](bool value)
                    {
                        hasForward = value;
                    });
                auto optionsObserver = Observer::Value<File::DirectoryListOptions>::create(
                    model->observeOptions(),
                    [&options](const File::DirectoryListOptions& value)
                    {
                        options = value;
                    });
                
                model->setPath(getTempPath());
                {
                    _print("Path: " + path.get());
                }
                model->reload();
                for (const auto& i : info)
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

                File::DirectoryListOptions optionsA;
                optionsA.extensions.insert(".txt");
                optionsA.sequences = true;
                optionsA.sequenceExtensions.insert(".txt");
                optionsA.showHidden = true;
                optionsA.sort = File::DirectoryListSort::Size;
                optionsA.reverseSort = true;
                optionsA.sortDirectoriesFirst = false;
                optionsA.filter = ".txt";
                model->setOptions(optionsA);
                DJV_ASSERT(options == optionsA);
                
                _tickFor(std::chrono::milliseconds(1000));

                auto io = File::IO::create();
                io->open(
                    File::Path(path, "file.txt").get(),
                    File::IO::Mode::Write);
                io->close();

                _tickFor(std::chrono::milliseconds(1000));
            }
        }
        
    } // namespace SystemTest
} // namespace djv

