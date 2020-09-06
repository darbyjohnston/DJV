// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/AnimationTest.h>
#include <djvCoreTest/BBoxTest.h>
#include <djvCoreTest/CacheTest.h>
#include <djvCoreTest/ContextTest.h>
#include <djvCoreTest/DirectoryModelTest.h>
#include <djvCoreTest/DirectoryWatcherTest.h>
#include <djvCoreTest/DrivesModelTest.h>
#include <djvCoreTest/EnumTest.h>
#include <djvCoreTest/ErrorTest.h>
#include <djvCoreTest/EventTest.h>
#include <djvCoreTest/FileIOTest.h>
#include <djvCoreTest/FileInfoTest.h>
#include <djvCoreTest/FileSystemTest.h>
#include <djvCoreTest/FrameNumberTest.h>
#include <djvCoreTest/IEventSystemTest.h>
#include <djvCoreTest/ISystemTest.h>
#include <djvCoreTest/ListObserverTest.h>
#include <djvCoreTest/LogSystemTest.h>
#include <djvCoreTest/MapObserverTest.h>
#include <djvCoreTest/MathTest.h>
#include <djvCoreTest/MatrixTest.h>
#include <djvCoreTest/MemoryTest.h>
#include <djvCoreTest/NumericValueModelsTest.h>
#include <djvCoreTest/OSTest.h>
#include <djvCoreTest/ObjectTest.h>
#include <djvCoreTest/PathTest.h>
#include <djvCoreTest/RangeTest.h>
#include <djvCoreTest/RapidJSONTest.h>
#include <djvCoreTest/RationalTest.h>
#include <djvCoreTest/RecentFilesModelTest.h>
#include <djvCoreTest/SpeedTest.h>
#include <djvCoreTest/StringFormatTest.h>
#include <djvCoreTest/StringTest.h>
#include <djvCoreTest/TextSystemTest.h>
#include <djvCoreTest/TimeTest.h>
#include <djvCoreTest/TimerTest.h>
#include <djvCoreTest/UIDTest.h>
#include <djvCoreTest/UndoStackTest.h>
#include <djvCoreTest/ValueObserverTest.h>
#include <djvCoreTest/VectorTest.h>

#include <djvAVTest/AVSystemTest.h>
#include <djvAVTest/AudioDataTest.h>
#include <djvAVTest/AudioTest.h>
#include <djvAVTest/ColorTest.h>
#include <djvAVTest/EnumTest.h>
#include <djvAVTest/FontSystemTest.h>
#include <djvAVTest/IOTest.h>
#include <djvAVTest/ImageConvertTest.h>
#include <djvAVTest/ImageDataTest.h>
#include <djvAVTest/ImageTest.h>
#include <djvAVTest/OCIOSystemTest.h>
#include <djvAVTest/OCIOTest.h>
#include <djvAVTest/PixelTest.h>
#include <djvAVTest/Render2DTest.h>
#include <djvAVTest/ThumbnailSystemTest.h>
#include <djvAVTest/TagsTest.h>

#include <djvUITest/ActionGroupTest.h>
#include <djvUITest/ButtonGroupTest.h>
#include <djvUITest/EnumTest.h>
#include <djvUITest/WidgetTest.h>

#if !defined(DJV_BUILD_TINY) && !defined(DJV_BUILD_MINIMAL)
#include <djvViewAppTest/FileSystemTest.h>
#endif

#include <djvUI/UISystem.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/Path.h>
#include <djvCore/FileInfo.h>

#include <iostream>

using namespace djv;
using namespace djv::Core;

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        auto context = Context::create(argv[0]);
        auto avSystem = AV::AVSystem::create(context);
        auto uiSystem = UI::UISystem::create(true, context);
        
        //! \bug This path should be unique to this process.
        const FileSystem::Path tempPath(FileSystem::Path::getTemp(), "djvTest");
        std::cout << "temp path: " << tempPath.get() << std::endl;

        std::vector<std::shared_ptr<Test::ITest> > tests;
        
        tests.emplace_back(new CoreTest::AnimationTest(tempPath, context));
        tests.emplace_back(new CoreTest::BBoxTest(tempPath, context));
        tests.emplace_back(new CoreTest::CacheTest(tempPath, context));
        tests.emplace_back(new CoreTest::ContextTest(tempPath, context));
        tests.emplace_back(new CoreTest::DirectoryModelTest(tempPath, context));
        tests.emplace_back(new CoreTest::DirectoryWatcherTest(tempPath, context));
        tests.emplace_back(new CoreTest::DrivesModelTest(tempPath, context));
        tests.emplace_back(new CoreTest::EnumTest(tempPath, context));
        tests.emplace_back(new CoreTest::ErrorTest(tempPath, context));
        tests.emplace_back(new CoreTest::EventTest(tempPath, context));
        tests.emplace_back(new CoreTest::FileIOTest(tempPath, context));
        tests.emplace_back(new CoreTest::FileInfoTest(tempPath, context));
        tests.emplace_back(new CoreTest::FileSystemTest(tempPath, context));
        tests.emplace_back(new CoreTest::FrameNumberTest(tempPath, context));
        tests.emplace_back(new CoreTest::IEventSystemTest(tempPath, context));
        tests.emplace_back(new CoreTest::ISystemTest(tempPath, context));
        tests.emplace_back(new CoreTest::ListObserverTest(tempPath, context));
        tests.emplace_back(new CoreTest::LogSystemTest(tempPath, context));
        tests.emplace_back(new CoreTest::MapObserverTest(tempPath, context));
        tests.emplace_back(new CoreTest::MathTest(tempPath, context));
        tests.emplace_back(new CoreTest::MatrixTest(tempPath, context));
        tests.emplace_back(new CoreTest::MemoryTest(tempPath, context));
        tests.emplace_back(new CoreTest::NumericValueModelsTest(tempPath, context));
        tests.emplace_back(new CoreTest::OSTest(tempPath, context));
        tests.emplace_back(new CoreTest::ObjectTest(tempPath, context));
        tests.emplace_back(new CoreTest::PathTest(tempPath, context));
        tests.emplace_back(new CoreTest::RangeTest(tempPath, context));
        tests.emplace_back(new CoreTest::RapidJSONTest(tempPath, context));
        tests.emplace_back(new CoreTest::RationalTest(tempPath, context));
        tests.emplace_back(new CoreTest::RecentFilesModelTest(tempPath, context));
        tests.emplace_back(new CoreTest::SpeedTest(tempPath, context));
        tests.emplace_back(new CoreTest::StringFormatTest(tempPath, context));
        tests.emplace_back(new CoreTest::StringTest(tempPath, context));
        tests.emplace_back(new CoreTest::TextSystemTest(tempPath, context));
        tests.emplace_back(new CoreTest::TimeTest(tempPath, context));
        tests.emplace_back(new CoreTest::TimerTest(tempPath, context));
        tests.emplace_back(new CoreTest::UIDTest(tempPath, context));
        tests.emplace_back(new CoreTest::UndoStackTest(tempPath, context));
        tests.emplace_back(new CoreTest::ValueObserverTest(tempPath, context));
        tests.emplace_back(new CoreTest::VectorTest(tempPath, context));

        tests.emplace_back(new AVTest::AVSystemTest(tempPath, context));
        tests.emplace_back(new AVTest::AudioDataTest(tempPath, context));
        tests.emplace_back(new AVTest::AudioTest(tempPath, context));
        tests.emplace_back(new AVTest::ColorTest(tempPath, context));
        tests.emplace_back(new AVTest::EnumTest(tempPath, context));
        tests.emplace_back(new AVTest::FontSystemTest(tempPath, context));
        tests.emplace_back(new AVTest::IOTest(tempPath, context));
        tests.emplace_back(new AVTest::ImageConvertTest(tempPath, context));
        tests.emplace_back(new AVTest::ImageDataTest(tempPath, context));
        tests.emplace_back(new AVTest::ImageTest(tempPath, context));
        tests.emplace_back(new AVTest::OCIOSystemTest(tempPath, context));
        tests.emplace_back(new AVTest::OCIOTest(tempPath, context));
        tests.emplace_back(new AVTest::PixelTest(tempPath, context));
        tests.emplace_back(new AVTest::Render2DTest(tempPath, context));
        tests.emplace_back(new AVTest::ThumbnailSystemTest(tempPath, context));
        tests.emplace_back(new AVTest::TagsTest(tempPath, context));

        tests.emplace_back(new UITest::ActionGroupTest(tempPath, context));
        tests.emplace_back(new UITest::ButtonGroupTest(tempPath, context));
        tests.emplace_back(new UITest::EnumTest(tempPath, context));
        tests.emplace_back(new UITest::WidgetTest(tempPath, context));

#if !defined(DJV_BUILD_TINY) && !defined(DJV_BUILD_MINIMAL)
        tests.emplace_back(new ViewAppTest::FileSystemTest(tempPath, context));
#endif

        std::vector<std::shared_ptr<Test::ITest> > testsToRun;
        if (1 == argc)
        {
            for (auto& i : tests)
            {
                testsToRun.push_back(i);
            }
        }
        else
        {
            for (int i = 1; i < argc; ++i)
            {
                const std::string name(argv[i]);
                for (const auto& j : tests)
                {
                    if (String::match(j->getName(), name))
                    {
                        testsToRun.push_back(j);
                    }
                }
            }
        }

        for (const auto& i : testsToRun)
        {
            i->run();
        }
    }
    catch (const std::exception & error)
    {
        std::cout << Error::format(error) << std::endl;
        DJV_ASSERT(false);
    }
    return r;
}

