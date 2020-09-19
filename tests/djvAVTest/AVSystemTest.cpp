// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/AVSystemTest.h>

#include <djvAV/AVSystem.h>
#include <djvAV/SpeedFunc.h>
#include <djvAV/TimeFunc.h>

#include <djvSystem/Context.h>

#include <djvCore/ValueObserver.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        AVSystemTest::AVSystemTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::AVTest::AVSystemTest", tempPath, context)
        {}
        
        void AVSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<AVSystem>();
                
                auto timeUnitsObserver = ValueObserver<AV::Time::Units>::create(
                    system->observeTimeUnits(),
                    [this](AV::Time::Units value)
                    {
                        std::stringstream ss;
                        ss << "Time units: " << value;
                        _print(ss.str());
                    });
                auto defaultSpeedObserver = ValueObserver<FPS>::create(
                    system->observeDefaultSpeed(),
                    [this](FPS value)
                    {
                        std::stringstream ss;
                        ss << "Default speed: " << value;
                        _print(ss.str());
                    });

                system->setTimeUnits(AV::Time::Units::Frames);
                system->setDefaultSpeed(FPS::_60);
            }
        }

    } // namespace AVTest
} // namespace djv

