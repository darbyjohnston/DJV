// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewAppTest/FileSystemTest.h>

#include <djvViewApp/FileSystem.h>

#include <djvDesktopApp/EventSystem.h>
#include <djvDesktopApp/GLFWSystem.h>

#include <djvUIComponents/UIComponentsSystem.h>

#include <djvUI/UISystem.h>

#include <djvAV/GLFWSystem.h>
#include <djvAV/IO.h>

#include <djvCore/Context.h>

using namespace djv::Core;
using namespace djv::ViewApp;

namespace djv
{
    namespace ViewAppTest
    {        
        FileSystemTest::FileSystemTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::FileSystemTest::FileSystemTest", context)
        {
            auto glfwSystem = Desktop::GLFWSystem::create(context);
            auto uiSystem = UI::UISystem::create(true, context);
            auto avGLFWSystem = context->getSystemT<AV::GLFW::System>();
            auto glfwWindow = avGLFWSystem->getGLFWWindow();
            auto eventSystem = Desktop::EventSystem::create(glfwWindow, context);
            UI::UIComponentsSystem::create(context);
        }
        
        void FileSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                {
                    auto fileSystem = ViewApp::FileSystem::create(context);

                    const auto fileInfoA = Core::FileSystem::FileInfo(
                        Core::FileSystem::Path("ViewAppTest_FileSystemTest_A.1.png"),
                        Core::FileSystem::FileType::Sequence,
                        Frame::Sequence(1, 3));
                    _writeImage(fileInfoA);
                    const auto fileInfoB = Core::FileSystem::FileInfo(
                        Core::FileSystem::Path("ViewAppTest_FileSystemTest_B.1.png"),
                        Core::FileSystem::FileType::Sequence,
                        Frame::Sequence(3, 6));
                    _writeImage(fileInfoB);

                    bool openObserved = false;
                    auto openObserver = ValueObserver<std::shared_ptr<Media> >::create(
                        fileSystem->observeOpened(),
                        [&openObserved](const std::shared_ptr<Media>& value)
                    {
                        openObserved = true;
                    });
                    bool mediaObserved = false;
                    std::vector<std::shared_ptr<Media> > media;
                    auto mediaObserver = ListObserver<std::shared_ptr<Media> >::create(
                        fileSystem->observeMedia(),
                        [&mediaObserved, &media](const std::vector<std::shared_ptr<Media> >& value)
                    {
                        mediaObserved = true;
                        media = value;
                    });
                    bool currentMediaObserved = false;
                    std::shared_ptr<Media> currentMedia;
                    auto currentMediaObserver = ValueObserver<std::shared_ptr<Media> >::create(
                        fileSystem->observeCurrentMedia(),
                        [&currentMediaObserved, &currentMedia](const std::shared_ptr<Media>& value)
                    {
                        currentMediaObserved = true;
                        currentMedia = value;
                    });
                    fileSystem->open(fileInfoA);
                    DJV_ASSERT(openObserved);
                    DJV_ASSERT(mediaObserved);
                    DJV_ASSERT(1 == media.size());
                    DJV_ASSERT(currentMediaObserved);
                    DJV_ASSERT(media[0] == currentMedia);

                    bool closeObserved = false;
                    auto closeObserver = ValueObserver<std::shared_ptr<Media> >::create(
                        fileSystem->observeClosed(),
                        [&closeObserved](const std::shared_ptr<Media>& value)
                    {
                        closeObserved = true;
                    });

                    fileSystem->close(media[0]);
                    DJV_ASSERT(closeObserved);
                    DJV_ASSERT(0 == media.size());

                    fileSystem->open(fileInfoA);
                    fileSystem->open(fileInfoB);
                    DJV_ASSERT(2 == media.size());
                    DJV_ASSERT(media[1] == currentMedia);
                    fileSystem->setCurrentMedia(media[0]);
                    DJV_ASSERT(media[0] == currentMedia);
                    closeObserved = false;
                    fileSystem->closeAll();
                    DJV_ASSERT(closeObserved);
                    DJV_ASSERT(0 == media.size());
                }
            }
        }

        void FileSystemTest::_writeImage(const Core::FileSystem::FileInfo& value)
        {
            if (auto context = getContext().lock())
            {
                const auto imageInfo = AV::Image::Info(AV::Image::Size(640, 480), AV::Image::Type::RGB_U8);
                auto image = AV::Image::Image::create(imageInfo);
                AV::IO::Info info;
                info.video.push_back(imageInfo);
                auto io = context->getSystemT<AV::IO::System>();
                auto write = io->write(value, info);
                {
                    std::lock_guard<std::mutex> lock(write->getMutex());
                    auto& writeQueue = write->getVideoQueue();
                    const auto& sequence = value.getSequence();
                    for (size_t i = 0; i < sequence.getSize(); ++i)
                    {
                        writeQueue.addFrame(AV::IO::VideoFrame(i, image));
                    }
                    writeQueue.setFinished(true);
                }
                while (write->isRunning())
                {}
            }
        }

    } // namespace ViewAppTest
} // namespace djv

