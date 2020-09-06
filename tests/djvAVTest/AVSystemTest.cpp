// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/AVSystemTest.h>

#include <djvAV/AVSystem.h>

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
                        ss << "time units: " << value;
                        _print(ss.str());
                    });
                auto alphaBlendObserver = ValueObserver<AlphaBlend>::create(
                    system->observeAlphaBlend(),
                    [this](AlphaBlend value)
                    {
                        std::stringstream ss;
                        ss << "alpha blend: " << value;
                        _print(ss.str());
                    });
                auto defaultSpeedObserver = ValueObserver<Time::FPS>::create(
                    system->observeDefaultSpeed(),
                    [this](Time::FPS value)
                    {
                        std::stringstream ss;
                        ss << "defaut speed: " << value;
                        _print(ss.str());
                    });

                system->setTimeUnits(Time::Units::Frames);
                system->setAlphaBlend(AlphaBlend::Premultiplied);
                system->setDefaultSpeed(Time::FPS::_60);
            }
        }

    } // namespace AVTest
} // namespace djv

