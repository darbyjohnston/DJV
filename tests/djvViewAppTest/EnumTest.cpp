// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewAppTest/EnumTest.h>

#include <djvViewApp/Enum.h>

using namespace djv::Core;
using namespace djv::ViewApp;

namespace djv
{
    namespace ViewAppTest
    {        
        EnumTest::EnumTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::ViewAppTest::EnumTest", tempPath, context)
        {}
        
        void EnumTest::run()
        {
            for (auto i : getViewLockEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("View lock: " + _getText(ss.str()));
            }
            
            for (auto i : getGridLabelsEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Grid labels: " + _getText(ss.str()));
            }
            
            for (auto i : getHUDBackgroundEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("HUD background: " + _getText(ss.str()));
            }
            
            for (auto i : getPlaybackEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Playback: " + _getText(ss.str()));
            }
            
            for (auto i : getPlaybackSpeedEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Playback speed: " + _getText(ss.str()));
            }
            
            for (auto i : getPlaybackModeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Playback mode: " + _getText(ss.str()));
            }
            
            for (auto i : getCmdLineModeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Command line mode: " + _getText(ss.str()));
            }
        }

    } // namespace ViewAppTest
} // namespace djv

