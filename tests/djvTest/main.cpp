// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/CacheTest.h>
#include <djvCoreTest/EnumTest.h>
#include <djvCoreTest/ErrorFuncTest.h>
#include <djvCoreTest/ListObserverTest.h>
#include <djvCoreTest/MapObserverTest.h>
#include <djvCoreTest/MemoryFuncTest.h>
#include <djvCoreTest/OSFuncTest.h>
#include <djvCoreTest/RandomFuncTest.h>
#include <djvCoreTest/RapidJSONFuncTest.h>
#include <djvCoreTest/StringFormatTest.h>
#include <djvCoreTest/StringFuncTest.h>
#include <djvCoreTest/TimeFuncTest.h>
#include <djvCoreTest/UIDFuncTest.h>
#include <djvCoreTest/UndoStackTest.h>
#include <djvCoreTest/ValueObserverTest.h>

#include <djvMathTest/BBoxFuncTest.h>
#include <djvMathTest/BBoxTest.h>
#include <djvMathTest/FrameNumberFuncTest.h>
#include <djvMathTest/FrameNumberTest.h>
#include <djvMathTest/MathFuncTest.h>
#include <djvMathTest/MatrixFuncTest.h>
#include <djvMathTest/NumericValueModelsTest.h>
#include <djvMathTest/RangeFuncTest.h>
#include <djvMathTest/RangeTest.h>
#include <djvMathTest/RationalFuncTest.h>
#include <djvMathTest/RationalTest.h>
#include <djvMathTest/VectorFuncTest.h>

#include <djvSystemTest/AnimationTest.h>
#include <djvSystemTest/AnimationFuncTest.h>
#include <djvSystemTest/ContextTest.h>
#include <djvSystemTest/DirectoryModelTest.h>
#include <djvSystemTest/DirectoryWatcherTest.h>
#include <djvSystemTest/DrivesModelTest.h>
#include <djvSystemTest/EventFuncTest.h>
#include <djvSystemTest/EventTest.h>
#include <djvSystemTest/FileFuncTest.h>
#include <djvSystemTest/FileIOFuncTest.h>
#include <djvSystemTest/FileIOTest.h>
#include <djvSystemTest/FileInfoFuncTest.h>
#include <djvSystemTest/FileInfoTest.h>
#include <djvSystemTest/IEventSystemTest.h>
#include <djvSystemTest/ISystemTest.h>
#include <djvSystemTest/LogSystemTest.h>
#include <djvSystemTest/ObjectTest.h>
#include <djvSystemTest/PathFuncTest.h>
#include <djvSystemTest/PathTest.h>
#include <djvSystemTest/RecentFilesModelTest.h>
#include <djvSystemTest/TextSystemTest.h>
#include <djvSystemTest/TimerFuncTest.h>
#include <djvSystemTest/TimerTest.h>

#include <djvImageTest/ColorFuncTest.h>
#include <djvImageTest/ColorTest.h>
#include <djvImageTest/ImageDataFuncTest.h>
#include <djvImageTest/ImageDataTest.h>
#include <djvImageTest/ImageTest.h>
#include <djvImageTest/ImageFuncTest.h>
#include <djvImageTest/PixelFuncTest.h>
#include <djvImageTest/PixelTest.h>
#include <djvImageTest/TagsTest.h>

#include <djvAudioTest/AudioDataTest.h>
#include <djvAudioTest/AudioFuncTest.h>
#include <djvAudioTest/AudioTest.h>
#include <djvAudioTest/AudioSystemFuncTest.h>
#include <djvAudioTest/AudioSystemTest.h>

#include <djvGeomTest/ShapeTest.h>
#include <djvGeomTest/TriangleMeshFuncTest.h>
#include <djvGeomTest/TriangleMeshTest.h>

#include <djvGLTest/EnumFuncTest.h>
#include <djvGLTest/ImageConvertTest.h>
#include <djvGLTest/MeshCacheTest.h>
#include <djvGLTest/MeshFuncTest.h>
#include <djvGLTest/MeshTest.h>
#include <djvGLTest/OffscreenBufferFuncTest.h>
#include <djvGLTest/OffscreenBufferTest.h>
#include <djvGLTest/ShaderTest.h>
#include <djvGLTest/TextureTest.h>
#include <djvGLTest/TextureAtlasTest.h>

#include <djvOCIOTest/OCIOSystemFuncTest.h>
#include <djvOCIOTest/OCIOSystemTest.h>
#include <djvOCIOTest/OCIOTest.h>

#include <djvRender2DTest/DataFuncTest.h>
#include <djvRender2DTest/DataTest.h>
#include <djvRender2DTest/EnumFuncTest.h>
#include <djvRender2DTest/FontSystemTest.h>
#include <djvRender2DTest/Render2DSystemTest.h>
#include <djvRender2DTest/RenderTest.h>

#include <djvRender3DTest/CameraTest.h>
#include <djvRender3DTest/LightTest.h>
#include <djvRender3DTest/MaterialTest.h>
#include <djvRender3DTest/RenderTest.h>

#include <djvAVTest/AVSystemTest.h>
#include <djvAVTest/CineonFuncTest.h>
#include <djvAVTest/DPXFuncTest.h>
#include <djvAVTest/IOTest.h>
#include <djvAVTest/PPMFuncTest.h>
#include <djvAVTest/SpeedFuncTest.h>
#include <djvAVTest/ThumbnailSystemTest.h>
#include <djvAVTest/TimeFuncTest.h>
#if !defined(DJV_BUILD_TINY) && !defined(DJV_BUILD_MINIMAL)
#include <djvAVTest/FFmpegFuncTest.h>
#include <djvAVTest/JPEGFuncTest.h>
#include <djvAVTest/OpenEXRFuncTest.h>
#include <djvAVTest/TIFFFuncTest.h>
#endif

#include <djvUITest/ActionGroupTest.h>
#include <djvUITest/ButtonGroupTest.h>
#include <djvUITest/EnumFuncTest.h>
#include <djvUITest/SelectionModelTest.h>
#include <djvUITest/WidgetTest.h>

#if !defined(DJV_BUILD_TINY) && !defined(DJV_BUILD_MINIMAL)
#include <djvViewAppTest/FileSystemTest.h>
#endif

#include <djvUI/UISystem.h>

#include <djvAV/AVSystem.h>

#include <djvRender3D/Render.h>

#include <djvRender2D/Render2DSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfo.h>
#include <djvSystem/PathFunc.h>

#include <djvCore/ErrorFunc.h>
#include <djvCore/StringFunc.h>

#include <iostream>

using namespace djv;
using namespace djv::Core;

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        auto context = System::Context::create(argv[0]);
        auto avSystem = AV::AVSystem::create(context);
        auto render2DSystem = Render2D::Render2DSystem::create(context);
        auto render3DSystem = Render3D::Render::create(context);
        auto uiSystem = UI::UISystem::create(true, context);
        
        //! \bug This path should be unique to this process.
        const System::File::Path tempPath(System::File::getTemp(), "djvTest");
        std::cout << "Temp path: " << tempPath.get() << std::endl;

        std::vector<std::shared_ptr<Test::ITest> > tests;
        
        tests.emplace_back(new CoreTest::CacheTest(tempPath, context));
        tests.emplace_back(new CoreTest::EnumTest(tempPath, context));
        tests.emplace_back(new CoreTest::ErrorFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::ListObserverTest(tempPath, context));
        tests.emplace_back(new CoreTest::MapObserverTest(tempPath, context));
        tests.emplace_back(new CoreTest::MemoryFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::OSFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::RandomFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::RapidJSONFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::StringFormatTest(tempPath, context));
        tests.emplace_back(new CoreTest::StringFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::TimeFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::UIDFuncTest(tempPath, context));
        tests.emplace_back(new CoreTest::UndoStackTest(tempPath, context));
        tests.emplace_back(new CoreTest::ValueObserverTest(tempPath, context));

        tests.emplace_back(new MathTest::BBoxFuncTest(tempPath, context));
        tests.emplace_back(new MathTest::BBoxTest(tempPath, context));
        tests.emplace_back(new MathTest::FrameNumberFuncTest(tempPath, context));
        tests.emplace_back(new MathTest::FrameNumberTest(tempPath, context));
        tests.emplace_back(new MathTest::MathFuncTest(tempPath, context));
        tests.emplace_back(new MathTest::MatrixFuncTest(tempPath, context));
        tests.emplace_back(new MathTest::NumericValueModelsTest(tempPath, context));
        tests.emplace_back(new MathTest::RangeFuncTest(tempPath, context));
        tests.emplace_back(new MathTest::RangeTest(tempPath, context));
        tests.emplace_back(new MathTest::RationalFuncTest(tempPath, context));
        tests.emplace_back(new MathTest::RationalTest(tempPath, context));
        tests.emplace_back(new MathTest::VectorFuncTest(tempPath, context));

        tests.emplace_back(new SystemTest::AnimationTest(tempPath, context));
        tests.emplace_back(new SystemTest::AnimationFuncTest(tempPath, context));
        tests.emplace_back(new SystemTest::ContextTest(tempPath, context));
        tests.emplace_back(new SystemTest::DirectoryModelTest(tempPath, context));
        tests.emplace_back(new SystemTest::DirectoryWatcherTest(tempPath, context));
        tests.emplace_back(new SystemTest::DrivesModelTest(tempPath, context));
        tests.emplace_back(new SystemTest::EventFuncTest(tempPath, context));
        tests.emplace_back(new SystemTest::EventTest(tempPath, context));
        tests.emplace_back(new SystemTest::FileFuncTest(tempPath, context));
        tests.emplace_back(new SystemTest::FileIOFuncTest(tempPath, context));
        tests.emplace_back(new SystemTest::FileIOTest(tempPath, context));
        tests.emplace_back(new SystemTest::FileInfoFuncTest(tempPath, context));
        tests.emplace_back(new SystemTest::FileInfoTest(tempPath, context));
        tests.emplace_back(new SystemTest::IEventSystemTest(tempPath, context));
        tests.emplace_back(new SystemTest::ISystemTest(tempPath, context));
        tests.emplace_back(new SystemTest::LogSystemTest(tempPath, context));
        tests.emplace_back(new SystemTest::ObjectTest(tempPath, context));
        tests.emplace_back(new SystemTest::PathFuncTest(tempPath, context));
        tests.emplace_back(new SystemTest::PathTest(tempPath, context));
        tests.emplace_back(new SystemTest::RecentFilesModelTest(tempPath, context));
        tests.emplace_back(new SystemTest::TextSystemTest(tempPath, context));
        tests.emplace_back(new SystemTest::TimerFuncTest(tempPath, context));
        tests.emplace_back(new SystemTest::TimerTest(tempPath, context));

        tests.emplace_back(new ImageTest::ColorFuncTest(tempPath, context));
        tests.emplace_back(new ImageTest::ColorTest(tempPath, context));
        tests.emplace_back(new ImageTest::ImageDataFuncTest(tempPath, context));
        tests.emplace_back(new ImageTest::ImageDataTest(tempPath, context));
        tests.emplace_back(new ImageTest::ImageTest(tempPath, context));
        tests.emplace_back(new ImageTest::ImageFuncTest(tempPath, context));
        tests.emplace_back(new ImageTest::PixelFuncTest(tempPath, context));
        tests.emplace_back(new ImageTest::PixelTest(tempPath, context));
        tests.emplace_back(new ImageTest::TagsTest(tempPath, context));

        tests.emplace_back(new AudioTest::AudioDataTest(tempPath, context));
        tests.emplace_back(new AudioTest::AudioFuncTest(tempPath, context));
        tests.emplace_back(new AudioTest::AudioTest(tempPath, context));
        tests.emplace_back(new AudioTest::AudioSystemFuncTest(tempPath, context));
        tests.emplace_back(new AudioTest::AudioSystemTest(tempPath, context));

        tests.emplace_back(new GeomTest::ShapeTest(tempPath, context));
        tests.emplace_back(new GeomTest::TriangleMeshFuncTest(tempPath, context));
        tests.emplace_back(new GeomTest::TriangleMeshTest(tempPath, context));

        tests.emplace_back(new GLTest::EnumFuncTest(tempPath, context));
        tests.emplace_back(new GLTest::ImageConvertTest(tempPath, context));
        tests.emplace_back(new GLTest::MeshCacheTest(tempPath, context));
        tests.emplace_back(new GLTest::MeshFuncTest(tempPath, context));
        tests.emplace_back(new GLTest::MeshTest(tempPath, context));
        tests.emplace_back(new GLTest::OffscreenBufferFuncTest(tempPath, context));
        tests.emplace_back(new GLTest::OffscreenBufferTest(tempPath, context));
        tests.emplace_back(new GLTest::ShaderTest(tempPath, context));
        tests.emplace_back(new GLTest::TextureAtlasTest(tempPath, context));
        tests.emplace_back(new GLTest::TextureTest(tempPath, context));

        tests.emplace_back(new OCIOTest::OCIOSystemFuncTest(tempPath, context));
        tests.emplace_back(new OCIOTest::OCIOSystemTest(tempPath, context));
        tests.emplace_back(new OCIOTest::OCIOTest(tempPath, context));

        tests.emplace_back(new Render2DTest::DataFuncTest(tempPath, context));
        tests.emplace_back(new Render2DTest::DataTest(tempPath, context));
        tests.emplace_back(new Render2DTest::EnumFuncTest(tempPath, context));
        tests.emplace_back(new Render2DTest::FontSystemTest(tempPath, context));
        tests.emplace_back(new Render2DTest::Render2DSystemTest(tempPath, context));
        tests.emplace_back(new Render2DTest::RenderTest(tempPath, context));

        tests.emplace_back(new Render3DTest::CameraTest(tempPath, context));
        tests.emplace_back(new Render3DTest::LightTest(tempPath, context));
        tests.emplace_back(new Render3DTest::MaterialTest(tempPath, context));
        tests.emplace_back(new Render3DTest::RenderTest(tempPath, context));

        tests.emplace_back(new AVTest::AVSystemTest(tempPath, context));
        tests.emplace_back(new AVTest::CineonFuncTest(tempPath, context));
        tests.emplace_back(new AVTest::DPXFuncTest(tempPath, context));
        tests.emplace_back(new AVTest::IOTest(tempPath, context));
        tests.emplace_back(new AVTest::PPMFuncTest(tempPath, context));
        tests.emplace_back(new AVTest::SpeedFuncTest(tempPath, context));
        tests.emplace_back(new AVTest::ThumbnailSystemTest(tempPath, context));
        tests.emplace_back(new AVTest::TimeFuncTest(tempPath, context));
#if !defined(DJV_BUILD_TINY) && !defined(DJV_BUILD_MINIMAL)
        tests.emplace_back(new AVTest::FFmpegFuncTest(tempPath, context));
        tests.emplace_back(new AVTest::JPEGFuncTest(tempPath, context));
        tests.emplace_back(new AVTest::OpenEXRFuncTest(tempPath, context));
        tests.emplace_back(new AVTest::TIFFFuncTest(tempPath, context));
#endif

        tests.emplace_back(new UITest::ActionGroupTest(tempPath, context));
        tests.emplace_back(new UITest::ButtonGroupTest(tempPath, context));
        tests.emplace_back(new UITest::EnumFuncTest(tempPath, context));
        tests.emplace_back(new UITest::SelectionModelTest(tempPath, context));
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

