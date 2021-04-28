// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/AVSystem.h>

#include <djvAV/IOSystem.h>
#include <djvAV/Speed.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvOCIO/OCIOSystem.h>

#include <djvGL/GLFWSystem.h>
#include <djvGL/ShaderSystem.h>

#include <djvAudio/AudioSystem.h>

#include <djvSystem/Context.h>

#include <iomanip>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        struct AVSystem::Private
        {
            std::shared_ptr<Observer::ValueSubject<Time::Units> > timeUnits;
            std::shared_ptr<Observer::ValueSubject<FPS> > defaultSpeed;
            std::shared_ptr<ThumbnailSystem> thumbnailSystem;
        };

        void AVSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            ISystem::_init("djv::AV::AVSystem", context);

            DJV_PRIVATE_PTR();
            p.timeUnits = Observer::ValueSubject<Time::Units>::create(Time::Units::First);
            p.defaultSpeed = Observer::ValueSubject<FPS>::create(getDefaultSpeed());

            auto audioSystem = Audio::AudioSystem::create(context);
            auto glfwSystem = GL::GLFW::GLFWSystem::create(context);
            auto shaderSystem = GL::ShaderSystem::create(context);
            auto ocioSystem = OCIO::OCIOSystem::create(context);
            auto ioSystem = IO::IOSystem::create(context);
            p.thumbnailSystem = ThumbnailSystem::create(context);
            addDependency(audioSystem);
            addDependency(glfwSystem);
            addDependency(shaderSystem);
            addDependency(ocioSystem);
            addDependency(ioSystem);
            addDependency(p.thumbnailSystem);

            _logInitTime();
        }

        AVSystem::AVSystem() :
            _p(new Private)
        {}

        AVSystem::~AVSystem()
        {}

        std::shared_ptr<AVSystem> AVSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<AVSystem>();
            if (!out)
            {
                out = std::shared_ptr<AVSystem>(new AVSystem);
                out->_init(context);
            }
            return out;
        }

        std::shared_ptr<Observer::IValueSubject<Time::Units> > AVSystem::observeTimeUnits() const
        {
            return _p->timeUnits;
        }

        std::shared_ptr<Observer::IValueSubject<FPS> > AVSystem::observeDefaultSpeed() const
        {
            return _p->defaultSpeed;
        }

        void AVSystem::setTimeUnits(Time::Units value)
        {
            _p->timeUnits->setIfChanged(value);
        }

        void AVSystem::setDefaultSpeed(FPS value)
        {
            DJV_PRIVATE_PTR();
            if (p.defaultSpeed->setIfChanged(value))
            {
                setDefaultSpeed(value);
                p.thumbnailSystem->clearCache();
            }
        }

    } // namespace AV
} // namespace djv

