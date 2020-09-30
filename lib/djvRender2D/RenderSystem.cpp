// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvRender2D/RenderSystem.h>

#include <djvRender2D/FontSystem.h>
#include <djvRender2D/Render.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace Render2D
    {
        struct RenderSystem::Private
        {
            std::shared_ptr<Observer::ValueSubject<ImageFilterOptions> > imageFilterOptions;
            std::shared_ptr<Observer::ValueSubject<bool> > textLCDRendering;
            std::shared_ptr<Font::FontSystem> fontSystem;
            std::shared_ptr<Render> render;
        };

        void RenderSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            ISystem::_init("djv::Render2D::RenderSystem", context);
            DJV_PRIVATE_PTR();

            p.imageFilterOptions = Observer::ValueSubject<ImageFilterOptions>::create();
            p.textLCDRendering = Observer::ValueSubject<bool>::create(true);

            p.fontSystem = Font::FontSystem::create(context);
            p.render = Render2D::Render::create(context);
            addDependency(p.fontSystem);
            addDependency(p.render);
        }

        RenderSystem::RenderSystem() :
            _p(new Private)
        {}

        RenderSystem::~RenderSystem()
        {}

        std::shared_ptr<RenderSystem> RenderSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<RenderSystem>();
            if (!out)
            {
                out = std::shared_ptr<RenderSystem>(new RenderSystem);
                out->_init(context);
            }
            return out;
        }

        std::shared_ptr<Observer::IValueSubject<ImageFilterOptions> > RenderSystem::observeImageFilterOptions() const
        {
            return _p->imageFilterOptions;
        }

        std::shared_ptr<Observer::IValueSubject<bool> > RenderSystem::observeTextLCDRendering() const
        {
            return _p->textLCDRendering;
        }

        void RenderSystem::setImageFilterOptions(const ImageFilterOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageFilterOptions->setIfChanged(value))
            {
                p.render->setImageFilterOptions(value);
            }
        }

        void RenderSystem::setTextLCDRendering(bool value)
        {
            DJV_PRIVATE_PTR();
            if (p.textLCDRendering->setIfChanged(value))
            {
                p.fontSystem->setLCDRendering(value);
                p.render->setTextLCDRendering(value);
            }
        }

    } // namespace Render2D
} // namespace djv

