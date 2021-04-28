// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/CacheTest.h>
#include <djvCoreTest/EnumTest.h>
#include <djvCoreTest/ErrorTest.h>
#include <djvCoreTest/ListObserverTest.h>
#include <djvCoreTest/MapObserverTest.h>
#include <djvCoreTest/MemoryTest.h>
#include <djvCoreTest/OSTest.h>
#include <djvCoreTest/RandomTest.h>
#include <djvCoreTest/RapidJSONTest.h>
#include <djvCoreTest/StringFormatTest.h>
#include <djvCoreTest/StringTest.h>
#include <djvCoreTest/TimeTest.h>
#include <djvCoreTest/UIDTest.h>
#include <djvCoreTest/UndoStackTest.h>
#include <djvCoreTest/ValueObserverTest.h>

#include <djvMathTest/BBoxTest.h>
#include <djvMathTest/FrameNumberTest.h>
#include <djvMathTest/MathTest.h>
#include <djvMathTest/MatrixTest.h>
#include <djvMathTest/NumericValueModelsTest.h>
#include <djvMathTest/RangeTest.h>
#include <djvMathTest/RationalTest.h>
#include <djvMathTest/VectorTest.h>

#include <djvSystemTest/AnimationTest.h>
#include <djvSystemTest/ContextTest.h>
#include <djvSystemTest/DirectoryModelTest.h>
#include <djvSystemTest/DirectoryWatcherTest.h>
#include <djvSystemTest/DrivesModelTest.h>
#include <djvSystemTest/EventTest.h>
#include <djvSystemTest/FileTest.h>
#include <djvSystemTest/FileIOTest.h>
#include <djvSystemTest/FileInfoTest.h>
#include <djvSystemTest/IEventSystemTest.h>
#include <djvSystemTest/ISystemTest.h>
#include <djvSystemTest/LogSystemTest.h>
#include <djvSystemTest/ObjectTest.h>
#include <djvSystemTest/PathTest.h>
#include <djvSystemTest/RecentFilesModelTest.h>
#include <djvSystemTest/TextSystemTest.h>
#include <djvSystemTest/TimerTest.h>

#include <djvImageTest/ColorTest.h>
#include <djvImageTest/DataTest.h>
#include <djvImageTest/InfoTest.h>
#include <djvImageTest/TagsTest.h>
#include <djvImageTest/TypeTest.h>

#include <djvAudioTest/AudioSystemTest.h>
#include <djvAudioTest/DataTest.h>
#include <djvAudioTest/InfoTest.h>
#include <djvAudioTest/TypeTest.h>

#include <djvGeomTest/ShapeTest.h>
#include <djvGeomTest/TriangleMeshTest.h>

#include <djvGLTest/EnumTest.h>
#include <djvGLTest/ImageConvertTest.h>
#include <djvGLTest/MeshCacheTest.h>
#include <djvGLTest/MeshTest.h>
#include <djvGLTest/OffscreenBufferTest.h>
#include <djvGLTest/ShaderTest.h>
#include <djvGLTest/TextureTest.h>
#include <djvGLTest/TextureAtlasTest.h>

#include <djvOCIOTest/OCIOSystemTest.h>
#include <djvOCIOTest/OCIOTest.h>

#include <djvRender2DTest/DataTest.h>
#include <djvRender2DTest/EnumTest.h>
#include <djvRender2DTest/FontSystemTest.h>
#include <djvRender2DTest/RenderSystemTest.h>
#include <djvRender2DTest/RenderTest.h>

#include <djvRender3DTest/CameraTest.h>
#include <djvRender3DTest/LightTest.h>
#include <djvRender3DTest/MaterialTest.h>
#include <djvRender3DTest/RenderTest.h>

#include <djvAVTest/AVSystemTest.h>
#include <djvAVTest/CineonTest.h>
#include <djvAVTest/DPXTest.h>
#include <djvAVTest/IOTest.h>
#include <djvAVTest/PPMTest.h>
#include <djvAVTest/SpeedTest.h>
#include <djvAVTest/ThumbnailSystemTest.h>
#include <djvAVTest/TimeTest.h>
#if defined(FFmpeg_FOUND)
#include <djvAVTest/FFmpegTest.h>
#endif // FFmpeg_FOUND
#if defined(JPEG_FOUND)
#include <djvAVTest/JPEGTest.h>
#endif // JPEG_FOUND
#if defined(OpenEXR_FOUND)
#include <djvAVTest/OpenEXRTest.h>
#endif // OpenEXR_FOUND
#if defined(TIFF_FOUND)
#include <djvAVTest/TIFFTest.h>
#endif // TIFF_FOUND

#include <djvUITest/ActionGroupTest.h>
#include <djvUITest/ButtonGroupTest.h>
#include <djvUITest/EnumTest.h>
#include <djvUITest/SelectionModelTest.h>
#include <djvUITest/WidgetTest.h>

#if !defined(DJV_BUILD_TINY) && !defined(DJV_BUILD_MINIMAL)
#include <djvViewAppTest/EnumTest.h>
#include <djvViewAppTest/FileSystemTest.h>
#endif

#include <djvUI/UISystem.h>

#include <djvScene3D/SceneSystem.h>

#include <djvRender3D/RenderSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfo.h>

#include <djvCore/Error.h>
#include <djvCore/String.h>

#include <iostream>

using namespace djv;
using namespace djv::Core;

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        auto context = System::Context::create(argv[0]);
        auto uiSystem = UI::UISystem::create(true, context);
        auto render3DSystem = Render3D::RenderSystem::create(context);

        //! \bug This path should be unique to this process.
        const System::File::Path tempPath(System::File::getTemp(), "djvTest");
        std::cout << "Temp path: " << tempPath.get() << std::endl;

        std::vector<std::shared_ptr<Test::ITest> > tests;
        
        tests.emplace_back(new CoreTest::CacheTest(tempPath, context));
        tests.emplace_back(new CoreTest::EnumTest(tempPath, context));
        tests.emplace_back(new CoreTest::ErrorTest(tempPath, context));
        tests.emplace_back(new CoreTest::ListObserverTest(tempPath, context));
        tests.emplace_back(new CoreTest::MapObserverTest(tempPath, context));
        tests.emplace_back(new CoreTest::MemoryTest(tempPath, context));
        tests.emplace_back(new CoreTest::OSTest(tempPath, context));
        tests.emplace_back(new CoreTest::RandomTest(tempPath, context));
        tests.emplace_back(new CoreTest::RapidJSONTest(tempPath, context));
        tests.emplace_back(new CoreTest::StringFormatTest(tempPath, context));
        tests.emplace_back(new CoreTest::StringTest(tempPath, context));
        tests.emplace_back(new CoreTest::TimeTest(tempPath, context));
        tests.emplace_back(new CoreTest::UIDTest(tempPath, context));
        tests.emplace_back(new CoreTest::UndoStackTest(tempPath, context));
        tests.emplace_back(new CoreTest::ValueObserverTest(tempPath, context));

        tests.emplace_back(new MathTest::BBoxTest(tempPath, context));
        tests.emplace_back(new MathTest::FrameNumberTest(tempPath, context));
        tests.emplace_back(new MathTest::MathTest(tempPath, context));
        tests.emplace_back(new MathTest::MatrixTest(tempPath, context));
        tests.emplace_back(new MathTest::NumericValueModelsTest(tempPath, context));
        tests.emplace_back(new MathTest::RangeTest(tempPath, context));
        tests.emplace_back(new MathTest::RationalTest(tempPath, context));
        tests.emplace_back(new MathTest::VectorTest(tempPath, context));

        tests.emplace_back(new SystemTest::AnimationTest(tempPath, context));
        tests.emplace_back(new SystemTest::ContextTest(tempPath, context));
        tests.emplace_back(new SystemTest::DirectoryModelTest(tempPath, context));
        tests.emplace_back(new SystemTest::DirectoryWatcherTest(tempPath, context));
        tests.emplace_back(new SystemTest::DrivesModelTest(tempPath, context));
        tests.emplace_back(new SystemTest::EventTest(tempPath, context));
        tests.emplace_back(new SystemTest::FileTest(tempPath, context));
        tests.emplace_back(new SystemTest::FileIOTest(tempPath, context));
        tests.emplace_back(new SystemTest::FileInfoTest(tempPath, context));
        tests.emplace_back(new SystemTest::IEventSystemTest(tempPath, context));
        tests.emplace_back(new SystemTest::ISystemTest(tempPath, context));
        tests.emplace_back(new SystemTest::LogSystemTest(tempPath, context));
        tests.emplace_back(new SystemTest::ObjectTest(tempPath, context));
        tests.emplace_back(new SystemTest::PathTest(tempPath, context));
        tests.emplace_back(new SystemTest::RecentFilesModelTest(tempPath, context));
        tests.emplace_back(new SystemTest::TextSystemTest(tempPath, context));
        tests.emplace_back(new SystemTest::TimerTest(tempPath, context));

        tests.emplace_back(new ImageTest::ColorTest(tempPath, context));
        tests.emplace_back(new ImageTest::DataTest(tempPath, context));
        tests.emplace_back(new ImageTest::InfoTest(tempPath, context));
        tests.emplace_back(new ImageTest::TypeTest(tempPath, context));
        tests.emplace_back(new ImageTest::TagsTest(tempPath, context));

        tests.emplace_back(new AudioTest::AudioSystemTest(tempPath, context));
        tests.emplace_back(new AudioTest::DataTest(tempPath, context));
        tests.emplace_back(new AudioTest::InfoTest(tempPath, context));
        tests.emplace_back(new AudioTest::TypeTest(tempPath, context));

        tests.emplace_back(new GeomTest::ShapeTest(tempPath, context));
        tests.emplace_back(new GeomTest::TriangleMeshTest(tempPath, context));

        tests.emplace_back(new GLTest::EnumTest(tempPath, context));
        tests.emplace_back(new GLTest::ImageConvertTest(tempPath, context));
        tests.emplace_back(new GLTest::MeshCacheTest(tempPath, context));
        tests.emplace_back(new GLTest::MeshTest(tempPath, context));
        tests.emplace_back(new GLTest::OffscreenBufferTest(tempPath, context));
        tests.emplace_back(new GLTest::ShaderTest(tempPath, context));
        tests.emplace_back(new GLTest::TextureAtlasTest(tempPath, context));
        tests.emplace_back(new GLTest::TextureTest(tempPath, context));

        tests.emplace_back(new OCIOTest::OCIOSystemTest(tempPath, context));
        tests.emplace_back(new OCIOTest::OCIOTest(tempPath, context));

        tests.emplace_back(new Render2DTest::DataTest(tempPath, context));
        tests.emplace_back(new Render2DTest::EnumTest(tempPath, context));
        tests.emplace_back(new Render2DTest::FontSystemTest(tempPath, context));
        tests.emplace_back(new Render2DTest::RenderSystemTest(tempPath, context));
        tests.emplace_back(new Render2DTest::RenderTest(tempPath, context));

        tests.emplace_back(new Render3DTest::CameraTest(tempPath, context));
        tests.emplace_back(new Render3DTest::LightTest(tempPath, context));
        tests.emplace_back(new Render3DTest::MaterialTest(tempPath, context));
        tests.emplace_back(new Render3DTest::RenderTest(tempPath, context));

        tests.emplace_back(new AVTest::AVSystemTest(tempPath, context));
        tests.emplace_back(new AVTest::CineonTest(tempPath, context));
        tests.emplace_back(new AVTest::DPXTest(tempPath, context));
        tests.emplace_back(new AVTest::IOTest(tempPath, context));
        tests.emplace_back(new AVTest::PPMTest(tempPath, context));
        tests.emplace_back(new AVTest::SpeedTest(tempPath, context));
        tests.emplace_back(new AVTest::ThumbnailSystemTest(tempPath, context));
        tests.emplace_back(new AVTest::TimeTest(tempPath, context));
#if defined(FFmpeg_FOUND)
        tests.emplace_back(new AVTest::FFmpegTest(tempPath, context));
#endif // FFmpeg_FOUND
#if defined(JPEG_FOUND)
        tests.emplace_back(new AVTest::JPEGTest(tempPath, context));
#endif // JPEG_FOUND
#if defined(OpenEXR_FOUND)
        tests.emplace_back(new AVTest::OpenEXRTest(tempPath, context));
#endif // OpenEXR_FOUND
#if defined(TIFF_FOUND)
        tests.emplace_back(new AVTest::TIFFTest(tempPath, context));
#endif // TIFF_FOUND

        tests.emplace_back(new UITest::ActionGroupTest(tempPath, context));
        tests.emplace_back(new UITest::ButtonGroupTest(tempPath, context));
        tests.emplace_back(new UITest::EnumTest(tempPath, context));
        tests.emplace_back(new UITest::SelectionModelTest(tempPath, context));
        tests.emplace_back(new UITest::WidgetTest(tempPath, context));

#if !defined(DJV_BUILD_TINY) && !defined(DJV_BUILD_MINIMAL)
        tests.emplace_back(new ViewAppTest::EnumTest(tempPath, context));
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

