//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAV/AVSystem.h>

#include <djvAV/AudioSystem.h>
#include <djvAV/FontSystem.h>
#include <djvAV/IO.h>
#include <djvAV/OCIOSystem.h>
#include <djvAV/Render2D.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/TextSystem.h>

#include <iomanip>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        struct AVSystem::Private
        {
            std::shared_ptr<ValueSubject<TimeUnits> > timeUnits;
        };

        void AVSystem::_init(Context * context)
        {
            ISystem::_init("djv::AV::AVSystem", context);

            DJV_PRIVATE_PTR();
            p.timeUnits = ValueSubject<TimeUnits>::create(TimeUnits::First);

            auto ioSystem = IO::System::create(context);
            auto ocioSystem = OCIOSystem::create(context);
            auto fontSystem = Font::System::create(context);
            auto thumbnailSystem = ThumbnailSystem::create(context);
            auto render2D = Render::Render2D::create(context);
            auto audioSystem = Audio::System::create(context);

            addDependency(ioSystem);
            addDependency(ocioSystem);
            addDependency(fontSystem);
            addDependency(thumbnailSystem);
            addDependency(render2D);
            addDependency(audioSystem);
        }

        AVSystem::AVSystem() :
            _p(new Private)
        {}

        AVSystem::~AVSystem()
        {}

        std::shared_ptr<AVSystem> AVSystem::create(Context * context)
        {
            auto out = std::shared_ptr<AVSystem>(new AVSystem);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<TimeUnits> > AVSystem::observeTimeUnits() const
        {
            return _p->timeUnits;
        }

        void AVSystem::setTimeUnits(TimeUnits value)
        {
            _p->timeUnits->setIfChanged(value);
        }

        std::string AVSystem::getLabel(Time::Timestamp value, const Time::Speed& speed) const
        {
            DJV_PRIVATE_PTR();
            std::string out;
            Frame::Index frame = Time::timestampToFrame(value, speed);
            switch (p.timeUnits->get())
            {
            case TimeUnits::Timecode:
            {
                const float speedF = Math::Rational::toFloat(speed);
                const int hours = static_cast<int>(frame / (speedF * 60 * 60));
                frame -= static_cast<int64_t>(hours * static_cast<double>(speedF)) * 60 * 60;
                const int minutes = static_cast<int>(frame / (speedF * 60));
                frame -= static_cast<int64_t>(minutes * static_cast<double>(speedF)) * 60;
                const int seconds = static_cast<int>(frame / speedF);
                frame -= static_cast<int64_t>(seconds * static_cast<double>(speedF));
                std::stringstream ss;
                ss << std::setfill('0') << std::setw(2) << hours;
                ss << std::setw(0) << ":";
                ss << std::setfill('0') << std::setw(2) << minutes;
                ss << std::setw(0) << ":";
                ss << std::setfill('0') << std::setw(2) << seconds;
                ss << std::setw(0) << ":";
                ss << std::setfill('0') << std::setw(2) << frame;
                out = ss.str();
                break;
            }
            case TimeUnits::Frames:
            {
                std::stringstream ss;
                ss << frame;
                out = ss.str();
                break;
            }
            default: break;
            }
            return out;
        }

    } // namespace AV
} // namespace djv

