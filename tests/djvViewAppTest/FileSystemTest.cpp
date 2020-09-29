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

#include <djvAV/IOSystem.h>

#include <djvGL/GLFWSystem.h>

#include <djvSystem/Context.h>

using namespace djv::Core;
using namespace djv::ViewApp;

namespace djv
{
    namespace ViewAppTest
    {        
        FileSystemTest::FileSystemTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest(
                "djv::ViewAppTest::FileSystemTest",
                System::File::Path(tempPath, "ViewAppFileSystemTest"),
                context)
        {
            UI::UIComponentsSystem::create(context);
            auto glfwSystem = Desktop::GLFWSystem::create(context);
            auto eventSystem = Desktop::EventSystem::create(context->getSystemT<GL::GLFW::GLFWSystem>()->getGLFWWindow(), context);
        }
        
        void FileSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                {
                    const auto fileInfoA = System::File::Info(
                        System::File::Path(getTempPath(), "testA.#.png"),
                        System::File::Type::Sequence,
                        Math::Frame::Sequence(1, 3));
                    _writeImage(fileInfoA);
                    const auto fileInfoB = System::File::Info(
                        System::File::Path(getTempPath(), "testB.#.png"),
                        System::File::Type::Sequence,
                        Math::Frame::Sequence(3, 6));
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
                    const System::File::Path path(getTempPath(), "wallpaper2960819.png");
                    _writeImage(System::File::Info(path));
                    auto fileSystem = ViewApp::FileSystem::create(context);
                    fileSystem->open(std::vector<std::string>({ path.get() }));
                    auto media = fileSystem->observeCurrentMedia()->get();
                    DJV_ASSERT(media->isValid());
                }
                {
                    const System::File::Path path(getTempPath(), "wallpaper2960819-test.png");
                    _writeImage(System::File::Info(path));
                    auto fileSystem = ViewApp::FileSystem::create(context);
                    fileSystem->open(std::vector<std::string>({ path.get() }));
                    auto media = fileSystem->observeCurrentMedia()->get();
                    DJV_ASSERT(media->isValid());
                }

                // https://github.com/darbyjohnston/DJV/issues/96
                {
                    const System::File::Path path(getTempPath(), "dump.png");
                    _writeImage(System::File::Info(path));
                    _writeImage(System::File::Info(System::File::Path(getTempPath(), "dump1.png")));
                    auto fileSystem = ViewApp::FileSystem::create(context);
                    fileSystem->open(std::vector<std::string>({ path.get() }));
                    auto media = fileSystem->observeCurrentMedia()->get();
                    DJV_ASSERT(media->isValid());
                }

                // https://github.com/darbyjohnston/DJV/issues/115
                {
                    const System::File::Path path(getTempPath(), "Screenshot from 2019-07-17 0-14.png");
                    _writeImage(System::File::Info(path));
                    auto fileSystem = ViewApp::FileSystem::create(context);
                    fileSystem->open(std::vector<std::string>({ path.get() }));
                    auto media = fileSystem->observeCurrentMedia()->get();
                    DJV_ASSERT(media->isValid());
                }
            }
        }

        void FileSystemTest::_writeImage(const System::File::Info& value)
        {
            if (auto context = getContext().lock())
            {
                const auto imageInfo = Image::Info(Image::Size(640, 480), Image::Type::RGB_U8);
                auto image = Image::Image::create(imageInfo);
                AV::IO::Info info;
                info.video.push_back(imageInfo);
                auto io = context->getSystemT<AV::IO::IOSystem>();
                auto write = io->write(value, info);
                {
                    std::lock_guard<std::mutex> lock(write->getMutex());
                    auto& writeQueue = write->getVideoQueue();
                    const auto& sequence = value.getSequence();
                    const size_t size = sequence.getFrameCount() > 1 ? sequence.getFrameCount() : 1;
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

