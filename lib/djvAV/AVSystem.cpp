// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/AVSystem.h>

#include <djvAV/AudioSystem.h>
#include <djvAV/FontSystem.h>
#include <djvAV/GLFWSystem.h>
#include <djvAV/IO.h>
#include <djvAV/OCIOSystem.h>
#include <djvAV/Render2D.h>
#include <djvAV/Render3D.h>
#include <djvAV/ShaderSystem.h>
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
            std::shared_ptr<ValueSubject<Time::Units> > timeUnits;
            std::shared_ptr<ValueSubject<AlphaBlend> > alphaBlend;
            std::shared_ptr<ValueSubject<Time::FPS> > defaultSpeed;
            std::shared_ptr<ValueSubject<Render2D::ImageFilterOptions> > imageFilterOptions;
            std::shared_ptr<ValueSubject<bool> > textLCDRendering;
            std::shared_ptr<Font::System> fontSystem;
            std::shared_ptr<ThumbnailSystem> thumbnailSystem;
            std::shared_ptr<Render2D::Render> render2D;
        };

        void AVSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISystem::_init("djv::AV::AVSystem", context);

            DJV_PRIVATE_PTR();
            p.timeUnits = ValueSubject<Time::Units>::create(Time::Units::First);
            p.alphaBlend = ValueSubject<AlphaBlend>::create(AlphaBlend::First);
            p.defaultSpeed = ValueSubject<Time::FPS>::create(Time::getDefaultSpeed());
            p.imageFilterOptions = ValueSubject<Render2D::ImageFilterOptions>::create();
            p.textLCDRendering = ValueSubject<bool>::create(true);

            auto glfwSystem = GLFW::System::create(context);
            auto ocioSystem = OCIO::System::create(context);
            auto ioSystem = IO::System::create(context);
            p.fontSystem = Font::System::create(context);
            p.thumbnailSystem = ThumbnailSystem::create(context);
            auto shaderSystem = Render::ShaderSystem::create(context);
            p.render2D = Render2D::Render::create(context);
            auto render3D = Render3D::Render::create(context);
            auto audioSystem = Audio::System::create(context);

            addDependency(glfwSystem);
            addDependency(ocioSystem);
            addDependency(ioSystem);
            addDependency(p.fontSystem);
            addDependency(p.thumbnailSystem);
            addDependency(shaderSystem);
            addDependency(p.render2D);
            addDependency(render3D);
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

        std::shared_ptr<IValueSubject<Time::Units> > AVSystem::observeTimeUnits() const
        {
            return _p->timeUnits;
        }

        void AVSystem::setTimeUnits(Time::Units value)
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

        std::shared_ptr<IValueSubject<Render2D::ImageFilterOptions> > AVSystem::observeImageFilterOptions() const
        {
            return _p->imageFilterOptions;
        }

        void AVSystem::setImageFilterOptions(const Render2D::ImageFilterOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageFilterOptions->setIfChanged(value))
            {
                p.render2D->setImageFilterOptions(value);
            }
        }

        std::shared_ptr<IValueSubject<bool> > AVSystem::observeTextLCDRendering() const
        {
            return _p->textLCDRendering;
        }

        void AVSystem::setTextLCDRendering(bool value)
        {
            DJV_PRIVATE_PTR();
            if (p.textLCDRendering->setIfChanged(value))
            {
                p.fontSystem->setLCDRendering(value);
                p.render2D->setTextLCDRendering(value);
            }
        }

    } // namespace AV
} // namespace djv

