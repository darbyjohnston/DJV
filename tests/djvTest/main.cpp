// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/AnimationTest.h>
#include <djvCoreTest/AnimationFuncTest.h>
#include <djvCoreTest/BBoxFuncTest.h>
#include <djvCoreTest/BBoxTest.h>
#include <djvCoreTest/CacheTest.h>
#include <djvCoreTest/ContextTest.h>
#include <djvCoreTest/DirectoryModelTest.h>
#include <djvCoreTest/DirectoryWatcherTest.h>
#include <djvCoreTest/DrivesModelTest.h>
#include <djvCoreTest/EnumTest.h>
#include <djvCoreTest/ErrorFuncTest.h>
#include <djvCoreTest/EventFuncTest.h>
#include <djvCoreTest/EventTest.h>
#include <djvCoreTest/FileIOFuncTest.h>
#include <djvCoreTest/FileIOTest.h>
#include <djvCoreTest/FileInfoFuncTest.h>
#include <djvCoreTest/FileInfoTest.h>
#include <djvCoreTest/FileSystemFuncTest.h>
#include <djvCoreTest/FrameNumberFuncTest.h>
#include <djvCoreTest/FrameNumberTest.h>
#include <djvCoreTest/IEventSystemTest.h>
#include <djvCoreTest/ISystemTest.h>
#include <djvCoreTest/ListObserverTest.h>
#include <djvCoreTest/LogSystemTest.h>
#include <djvCoreTest/MapObserverTest.h>
#include <djvCoreTest/MathFuncTest.h>
#include <djvCoreTest/MatrixFuncTest.h>
#include <djvCoreTest/MemoryFuncTest.h>
#include <djvCoreTest/NumericValueModelsTest.h>
#include <djvCoreTest/OSFuncTest.h>
#include <djvCoreTest/ObjectTest.h>
#include <djvCoreTest/PathFuncTest.h>
#include <djvCoreTest/PathTest.h>
#include <djvCoreTest/RangeFuncTest.h>
#include <djvCoreTest/RangeTest.h>
#include <djvCoreTest/RapidJSONFuncTest.h>
#include <djvCoreTest/RationalFuncTest.h>
#include <djvCoreTest/RationalTest.h>
#include <djvCoreTest/RecentFilesModelTest.h>
#include <djvCoreTest/SpeedFuncTest.h>
#include <djvCoreTest/StringFormatTest.h>
#include <djvCoreTest/StringFuncTest.h>
#include <djvCoreTest/TextSystemTest.h>
#include <djvCoreTest/TimeFuncTest.h>
#include <djvCoreTest/TimerFuncTest.h>
#include <djvCoreTest/TimerTest.h>
#include <djvCoreTest/UIDFuncTest.h>
#include <djvCoreTest/UndoStackTest.h>
#include <djvCoreTest/ValueObserverTest.h>
#include <djvCoreTest/VectorFuncTest.h>

#include <djvAVTest/AVSystemTest.h>
#include <djvAVTest/AudioDataTest.h>
#include <djvAVTest/AudioSystemTest.h>
#include <djvAVTest/AudioTest.h>
#include <djvAVTest/CineonTest.h>
#include <djvAVTest/ColorTest.h>
#include <djvAVTest/DPXTest.h>
#include <djvAVTest/EnumTest.h>
#include <djvAVTest/FontSystemTest.h>
#include <djvAVTest/IOTest.h>
#include <djvAVTest/ImageConvertTest.h>
#include <djvAVTest/ImageDataTest.h>
#include <djvAVTest/ImageTest.h>
#include <djvAVTest/ImageUtilTest.h>
#include <djvAVTest/OCIOSystemTest.h>
#include <djvAVTest/OCIOTest.h>
#include <djvAVTest/OpenGLMeshCacheTest.h>
#include <djvAVTest/OpenGLMeshTest.h>
#include <djvAVTest/OpenGLOffscreenBufferTest.h>
#include <djvAVTest/OpenGLShaderTest.h>
#include <djvAVTest/OpenGLTextureTest.h>
#include <djvAVTest/OpenGLTextureAtlasTest.h>
#include <djvAVTest/PPMTest.h>
#include <djvAVTest/PixelTest.h>
#include <djvAVTest/Render2DTest.h>
#include <djvAVTest/Render3DCameraTest.h>
#include <djvAVTest/Render3DLightTest.h>
#include <djvAVTest/Render3DMaterialTest.h>
#include <djvAVTest/Render3DTest.h>
#include <djvAVTest/ShaderTest.h>
#include <djvAVTest/ShapeTest.h>
#include <djvAVTest/TagsTest.h>
#include <djvAVTest/ThumbnailSystemTest.h>
#include <djvAVTest/TriangleMeshTest.h>
#if !defined(DJV_BUILD_TINY) && !defined(DJV_BUILD_MINIMAL)
#include <djvAVTest/FFmpegTest.h>
#include <djvAVTest/JPEGTest.h>
#include <djvAVTest/OpenEXRTest.h>
#include <djvAVTest/TIFFTest.h>
#endif

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
#include <djvCore/ErrorFunc.h>
#include <djvCore/FileInfo.h>
#include <djvCore/PathFunc.h>
#include <djvCore/StringFunc.h>

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
        const FileSystem::Path tempPath(FileSystem::getTemp(), "djvTest");
        std::cout << "Temp path: " << tempPath.get() << std::endl;

        std::vector<std::shared_ptr<Test::ITest> > tests;
        
        tests.emplace_back(new CoreTest::AnimationTest(tempPath, context));
        tests.emplace_back(new CoreTest::AnimationFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::BBoxFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::BBoxTest(tempPath, context));
        tests.emplace_back(new CoreTest::CacheTest(tempPath, context));
        tests.emplace_back(new CoreTest::ContextTest(tempPath, context));
        tests.emplace_back(new CoreTest::DirectoryModelTest(tempPath, context));
        tests.emplace_back(new CoreTest::DirectoryWatcherTest(tempPath, context));
        tests.emplace_back(new CoreTest::DrivesModelTest(tempPath, context));
        tests.emplace_back(new CoreTest::EnumTest(tempPath, context));
        tests.emplace_back(new CoreTest::ErrorFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::EventFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::EventTest(tempPath, context));
        tests.emplace_back(new CoreTest::FileIOFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::FileIOTest(tempPath, context));
        tests.emplace_back(new CoreTest::FileInfoFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::FileInfoTest(tempPath, context));
        tests.emplace_back(new CoreTest::FileSystemFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::FrameNumberFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::FrameNumberTest(tempPath, context));
        tests.emplace_back(new CoreTest::IEventSystemTest(tempPath, context));
        tests.emplace_back(new CoreTest::ISystemTest(tempPath, context));
        tests.emplace_back(new CoreTest::ListObserverTest(tempPath, context));
        tests.emplace_back(new CoreTest::LogSystemTest(tempPath, context));
        tests.emplace_back(new CoreTest::MapObserverTest(tempPath, context));
        tests.emplace_back(new CoreTest::MathFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::MatrixFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::MemoryFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::NumericValueModelsTest(tempPath, context));
        tests.emplace_back(new CoreTest::OSFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::ObjectTest(tempPath, context));
        tests.emplace_back(new CoreTest::PathFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::PathTest(tempPath, context));
        tests.emplace_back(new CoreTest::RangeFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::RangeTest(tempPath, context));
        tests.emplace_back(new CoreTest::RapidJSONFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::RationalFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::RationalTest(tempPath, context));
        tests.emplace_back(new CoreTest::RecentFilesModelTest(tempPath, context));
        tests.emplace_back(new CoreTest::SpeedFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::StringFormatTest(tempPath, context));
        tests.emplace_back(new CoreTest::StringFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::TextSystemTest(tempPath, context));
        tests.emplace_back(new CoreTest::TimeFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::TimerFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::TimerTest(tempPath, context));
        tests.emplace_back(new CoreTest::UIDFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::UndoStackTest(tempPath, context));
        tests.emplace_back(new CoreTest::ValueObserverTest(tempPath, context));
        tests.emplace_back(new CoreTest::VectorFuncTest(tempPath, context));

        tests.emplace_back(new AVTest::AVSystemTest(tempPath, context));
        tests.emplace_back(new AVTest::AudioDataTest(tempPath, context));
        tests.emplace_back(new AVTest::AudioSystemTest(tempPath, context));
        tests.emplace_back(new AVTest::AudioTest(tempPath, context));
        tests.emplace_back(new AVTest::CineonTest(tempPath, context));
        tests.emplace_back(new AVTest::ColorTest(tempPath, context));
        tests.emplace_back(new AVTest::DPXTest(tempPath, context));
        tests.emplace_back(new AVTest::EnumTest(tempPath, context));
        tests.emplace_back(new AVTest::FontSystemTest(tempPath, context));
        tests.emplace_back(new AVTest::IOTest(tempPath, context));
        tests.emplace_back(new AVTest::ImageConvertTest(tempPath, context));
        tests.emplace_back(new AVTest::ImageDataTest(tempPath, context));
        tests.emplace_back(new AVTest::ImageTest(tempPath, context));
        tests.emplace_back(new AVTest::ImageUtilTest(tempPath, context));
        tests.emplace_back(new AVTest::OCIOSystemTest(tempPath, context));
        tests.emplace_back(new AVTest::OCIOTest(tempPath, context));
        tests.emplace_back(new AVTest::OpenGLMeshCacheTest(tempPath, context));
        tests.emplace_back(new AVTest::OpenGLMeshTest(tempPath, context));
        tests.emplace_back(new AVTest::OpenGLOffscreenBufferTest(tempPath, context));
        tests.emplace_back(new AVTest::OpenGLShaderTest(tempPath, context));
        tests.emplace_back(new AVTest::OpenGLTextureAtlasTest(tempPath, context));
        tests.emplace_back(new AVTest::OpenGLTextureTest(tempPath, context));
        tests.emplace_back(new AVTest::PPMTest(tempPath, context));
        tests.emplace_back(new AVTest::PixelTest(tempPath, context));
        tests.emplace_back(new AVTest::Render2DTest(tempPath, context));
        tests.emplace_back(new AVTest::Render3DCameraTest(tempPath, context));
        tests.emplace_back(new AVTest::Render3DLightTest(tempPath, context));
        tests.emplace_back(new AVTest::Render3DMaterialTest(tempPath, context));
        tests.emplace_back(new AVTest::Render3DTest(tempPath, context));
        tests.emplace_back(new AVTest::ShaderTest(tempPath, context));
        tests.emplace_back(new AVTest::ShapeTest(tempPath, context));
        tests.emplace_back(new AVTest::TagsTest(tempPath, context));
        tests.emplace_back(new AVTest::ThumbnailSystemTest(tempPath, context));
        tests.emplace_back(new AVTest::TriangleMeshTest(tempPath, context));
#if !defined(DJV_BUILD_TINY) && !defined(DJV_BUILD_MINIMAL)
        tests.emplace_back(new AVTest::FFmpegTest(tempPath, context));
        tests.emplace_back(new AVTest::JPEGTest(tempPath, context));
        tests.emplace_back(new AVTest::OpenEXRTest(tempPath, context));
        tests.emplace_back(new AVTest::TIFFTest(tempPath, context));
#endif

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

