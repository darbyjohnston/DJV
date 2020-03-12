//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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
        AVSystemTest::AVSystemTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::AVSystemTest", context)
        {}
        
        void AVSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<AVSystem>();
                
                auto timeUnitsObserver = ValueObserver<TimeUnits>::create(
                    system->observeTimeUnits(),
                    [this](TimeUnits value)
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

                {
                    std::stringstream ss;
                    ss << "frame number label: " << system->getLabel(100, Time::Speed());
                    _print(ss.str());
                }

                system->setTimeUnits(TimeUnits::Frames);
                system->setAlphaBlend(AlphaBlend::Premultiplied);
                system->setDefaultSpeed(Time::FPS::_60);

                {
                    std::stringstream ss;
                    ss << "frame number label: " << system->getLabel(100, Time::Speed());
                    _print(ss.str());
                }
            }
        }

    } // namespace AVTest
} // namespace djv

