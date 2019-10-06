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
#include <djvAV/GLFWSystem.h>
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
            std::shared_ptr<ValueSubject<AlphaBlend> > alphaBlend;
            std::shared_ptr<ValueSubject<Time::FPS> > defaultSpeed;
            std::shared_ptr<ThumbnailSystem> thumbnailSystem;
        };

        void AVSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISystem::_init("djv::AV::AVSystem", context);

            DJV_PRIVATE_PTR();
            p.timeUnits = ValueSubject<TimeUnits>::create(TimeUnits::First);
            p.alphaBlend = ValueSubject<AlphaBlend>::create(AlphaBlend::First);
            p.defaultSpeed = ValueSubject<Time::FPS>::create(Time::getDefaultSpeed());

            auto glfwSystem = GLFW::System::create(context);
            auto ocioSystem = OCIO::System::create(context);
            auto ioSystem = IO::System::create(context);
            auto fontSystem = Font::System::create(context);
            p.thumbnailSystem = ThumbnailSystem::create(context);
            auto render2D = Render::Render2D::create(context);
            auto audioSystem = Audio::System::create(context);

            addDependency(glfwSystem);
            addDependency(ocioSystem);
            addDependency(ioSystem);
            addDependency(fontSystem);
            addDependency(p.thumbnailSystem);
            addDependency(render2D);
            addDependency(audioSystem);
        }

        AVSystem::AVSystem() :
            _p(new Private)
        {}

        AVSystem::~AVSystem()
        {}

        std::shared_ptr<AVSystem> AVSystem::create(const std::shared_ptr<Core::Context>& context)
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

        std::shared_ptr<IValueSubject<AlphaBlend> > AVSystem::observeAlphaBlend() const
        {
            return _p->alphaBlend;
        }

        void AVSystem::setAlphaBlend(AlphaBlend value)
        {
            _p->alphaBlend->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<Time::FPS> > AVSystem::observeDefaultSpeed() const
        {
            return _p->defaultSpeed;
        }

        void AVSystem::setDefaultSpeed(Time::FPS value)
        {
            DJV_PRIVATE_PTR();
            if (p.defaultSpeed->setIfChanged(value))
            {
                Time::setDefaultSpeed(value);
                p.thumbnailSystem->clearCache();
            }
        }

        std::string AVSystem::getLabel(Frame::Number value, const Time::Speed& speed) const
        {
            DJV_PRIVATE_PTR();
            std::string out;
            switch (p.timeUnits->get())
            {
            case TimeUnits::Timecode:
            {
                const uint32_t timecode = Time::frameToTimecode(value, speed);
                out = Time::timecodeToString(timecode);
                break;
            }
            case TimeUnits::Frames:
            {
                std::stringstream ss;
                ss << value;
                out = ss.str();
                break;
            }
            default: break;
            }
            return out;
        }

    } // namespace AV
} // namespace djv

