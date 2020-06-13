// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewAppTest/FileSystemTest.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

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
                    const auto fileInfoA = Core::FileSystem::FileInfo(
                        Core::FileSystem::Path("testA.#.png"),
                        Core::FileSystem::FileType::Sequence,
                        Frame::Sequence(1, 3));
                    _writeImage(fileInfoA);
                    const auto fileInfoB = Core::FileSystem::FileInfo(
                        Core::FileSystem::Path("testB.#.png"),
                        Core::FileSystem::FileType::Sequence,
                        Frame::Sequence(3, 6));
                    _writeImage(fileInfoB);

                    auto fileSystem = ViewApp::FileSystem::create(context);
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

                // https://github.com/darbyjohnston/DJV/issues/31
                {
                    const std::string fileName = "wallpaper2960819.png";
                    _writeImage(Core::FileSystem::FileInfo(fileName));
                    auto fileSystem = ViewApp::FileSystem::create(context);
                    fileSystem->open(std::vector<std::string>({ fileName }));
                    auto media = fileSystem->observeCurrentMedia()->get();
                    DJV_ASSERT(media->isValid());
                }
                {
                    const std::string fileName = "wallpaper2960819-test.png";
                    _writeImage(Core::FileSystem::FileInfo(fileName));
                    auto fileSystem = ViewApp::FileSystem::create(context);
                    fileSystem->open(std::vector<std::string>({ fileName }));
                    auto media = fileSystem->observeCurrentMedia()->get();
                    DJV_ASSERT(media->isValid());
                }

                // https://github.com/darbyjohnston/DJV/issues/96
                {
                    const std::string fileName = "dump.png";
                    _writeImage(Core::FileSystem::FileInfo(fileName));
                    _writeImage(Core::FileSystem::FileInfo("dump1.png"));
                    auto fileSystem = ViewApp::FileSystem::create(context);
                    fileSystem->open(std::vector<std::string>({ fileName }));
                    auto media = fileSystem->observeCurrentMedia()->get();
                    DJV_ASSERT(media->isValid());
                    DJV_ASSERT(fileName == media->getFileInfo().getFileName(Frame::invalid, false));
                }

                // https://github.com/darbyjohnston/DJV/issues/115
                {
                    const std::string fileName = "Screenshot from 2019-07-17 0-14.png";
                    _writeImage(Core::FileSystem::FileInfo(fileName));
                    auto fileSystem = ViewApp::FileSystem::create(context);
                    fileSystem->open(std::vector<std::string>({ fileName }));
                    auto media = fileSystem->observeCurrentMedia()->get();
                    DJV_ASSERT(media->isValid());
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
                    const size_t size = sequence.getFrameCount() > 0 ? sequence.getFrameCount() : 1;
                    for (size_t i = 0; i < size; ++i)
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

