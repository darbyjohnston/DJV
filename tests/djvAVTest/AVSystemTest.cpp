// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/AVSystemTest.h>

#include <djvAV/AVSystem.h>
#include <djvAV/Render2DData.h>

#include <djvCore/Context.h>
#include <djvCore/ValueObserver.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        AVSystemTest::AVSystemTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::AVSystemTest", tempPath, context)
        {}
        
        void AVSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<AVSystem>();
                
                auto timeUnitsObserver = ValueObserver<Time::Units>::create(
                    system->observeTimeUnits(),
                    [this](Time::Units value)
                    {
                        std::stringstream ss;
                        ss << "Time units: " << value;
                        _print(ss.str());
                    });
                auto alphaBlendObserver = ValueObserver<AlphaBlend>::create(
                    system->observeAlphaBlend(),
                    [this](AlphaBlend value)
                    {
                        std::stringstream ss;
                        ss << "Alpha blend: " << value;
                        _print(ss.str());
                    });
                auto defaultSpeedObserver = ValueObserver<Time::FPS>::create(
                    system->observeDefaultSpeed(),
                    [this](Time::FPS value)
                    {
                        std::stringstream ss;
                        ss << "Default speed: " << value;
                        _print(ss.str());
                    });
                auto imageFilterOptionsObserver = ValueObserver<Render2D::ImageFilterOptions>::create(
                    system->observeImageFilterOptions(),
                    [this](const Render2D::ImageFilterOptions& value)
                    {
                        std::stringstream ss;
                        ss << "Image filter: " << value.min << " " << value.mag;
                        _print(ss.str());
                    });
                auto textLCDRenderingObserver = ValueObserver<bool>::create(
                    system->observeTextLCDRendering(),
                    [this](bool value)
                    {
                        std::stringstream ss;
                        ss << "Text LCD rendering: " << value;
                        _print(ss.str());
                    });

                system->setTimeUnits(Time::Units::Frames);
                system->setAlphaBlend(AlphaBlend::Premultiplied);
                system->setDefaultSpeed(Time::FPS::_60);
                system->setImageFilterOptions(Render2D::ImageFilterOptions(
                    Render2D::ImageFilter::Nearest,
                    Render2D::ImageFilter::Nearest));
                system->setTextLCDRendering(false);
            }
        }

    } // namespace AVTest
} // namespace djv

