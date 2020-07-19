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
#include <djvCoreTest/FrameTest.h>
#include <djvCoreTest/IEventSystemTest.h>
#include <djvCoreTest/ISystemTest.h>
#include <djvCoreTest/ListObserverTest.h>
#include <djvCoreTest/LogSystemTest.h>
#include <djvCoreTest/MapObserverTest.h>
#include <djvCoreTest/MathTest.h>
#include <djvCoreTest/MemoryTest.h>
#include <djvCoreTest/OSTest.h>
#include <djvCoreTest/ObjectTest.h>
#include <djvCoreTest/PathTest.h>
#include <djvCoreTest/RangeTest.h>
#include <djvCoreTest/RapidJSONTest.h>
#include <djvCoreTest/SpeedTest.h>
#include <djvCoreTest/StringFormatTest.h>
#include <djvCoreTest/StringTest.h>
#include <djvCoreTest/TextSystemTest.h>
#include <djvCoreTest/TimeTest.h>
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

#include <iostream>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        auto context = Core::Context::create(argv[0]);

        auto avSystem = AV::AVSystem::create(context);
        auto uiSystem = UI::UISystem::create(true, context);
        
        std::vector<std::shared_ptr<Test::ITest> > tests;
        if (1)
        {
            tests.emplace_back(new UITest::ActionGroupTest(context));
            tests.emplace_back(new UITest::ButtonGroupTest(context));
        }
        else
        {
            tests.emplace_back(new CoreTest::AnimationTest(context));
            tests.emplace_back(new CoreTest::BBoxTest(context));
            tests.emplace_back(new CoreTest::CacheTest(context));
            tests.emplace_back(new CoreTest::DirectoryModelTest(context));
            tests.emplace_back(new CoreTest::DirectoryWatcherTest(context));
            tests.emplace_back(new CoreTest::DrivesModelTest(context));
            tests.emplace_back(new CoreTest::EnumTest(context));
            tests.emplace_back(new CoreTest::ErrorTest(context));
            tests.emplace_back(new CoreTest::EventTest(context));
            tests.emplace_back(new CoreTest::FileIOTest(context));
            tests.emplace_back(new CoreTest::FileInfoTest(context));
            tests.emplace_back(new CoreTest::FrameTest(context));
            tests.emplace_back(new CoreTest::IEventSystemTest(context));
            tests.emplace_back(new CoreTest::ISystemTest(context));
            tests.emplace_back(new CoreTest::ListObserverTest(context));
            tests.emplace_back(new CoreTest::LogSystemTest(context));
            tests.emplace_back(new CoreTest::MapObserverTest(context));
            tests.emplace_back(new CoreTest::MathTest(context));
            tests.emplace_back(new CoreTest::MemoryTest(context));
            tests.emplace_back(new CoreTest::OSTest(context));
            tests.emplace_back(new CoreTest::ObjectTest(context));
            tests.emplace_back(new CoreTest::PathTest(context));
            tests.emplace_back(new CoreTest::RapidJSONTest(context));
            tests.emplace_back(new CoreTest::RangeTest(context));
            tests.emplace_back(new CoreTest::SpeedTest(context));
            tests.emplace_back(new CoreTest::StringFormatTest(context));
            tests.emplace_back(new CoreTest::StringTest(context));
            tests.emplace_back(new CoreTest::TextSystemTest(context));
            tests.emplace_back(new CoreTest::TimeTest(context));
            tests.emplace_back(new CoreTest::UndoStackTest(context));
            tests.emplace_back(new CoreTest::ValueObserverTest(context));
            tests.emplace_back(new CoreTest::VectorTest(context));

            tests.emplace_back(new AVTest::AVSystemTest(context));
            tests.emplace_back(new AVTest::AudioDataTest(context));
            tests.emplace_back(new AVTest::AudioTest(context));
            tests.emplace_back(new AVTest::ColorTest(context));
            tests.emplace_back(new AVTest::EnumTest(context));
            tests.emplace_back(new AVTest::FontSystemTest(context));
            tests.emplace_back(new AVTest::IOTest(context));
            tests.emplace_back(new AVTest::ImageConvertTest(context));
            tests.emplace_back(new AVTest::ImageDataTest(context));
            tests.emplace_back(new AVTest::ImageTest(context));
            tests.emplace_back(new AVTest::OCIOSystemTest(context));
            tests.emplace_back(new AVTest::OCIOTest(context));
            tests.emplace_back(new AVTest::PixelTest(context));
            tests.emplace_back(new AVTest::Render2DTest(context));
            tests.emplace_back(new AVTest::ThumbnailSystemTest(context));
            tests.emplace_back(new AVTest::TagsTest(context));

            tests.emplace_back(new UITest::ActionGroupTest(context));
            tests.emplace_back(new UITest::ButtonGroupTest(context));
            tests.emplace_back(new UITest::EnumTest(context));
            tests.emplace_back(new UITest::WidgetTest(context));

#if !defined(DJV_BUILD_TINY) && !defined(DJV_BUILD_MINIMAL)
            tests.emplace_back(new ViewAppTest::FileSystemTest(context));
#endif
        }

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
                    if (name == j->getName())
                    {
                        testsToRun.push_back(j);
                        break;
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
        std::cout << Core::Error::format(error) << std::endl;
        DJV_ASSERT(false);
    }
    return r;
}
