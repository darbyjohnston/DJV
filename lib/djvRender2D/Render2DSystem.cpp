// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvRender2D/Render2DSystem.h>

#include <djvRender2D/FontSystem.h>
#include <djvRender2D/Render.h>

using namespace djv::Core;

namespace djv
{
    namespace Render2D
    {
        struct Render2DSystem::Private
        {
            std::shared_ptr<ValueSubject<AlphaBlend> > alphaBlend;
            std::shared_ptr<ValueSubject<ImageFilterOptions> > imageFilterOptions;
            std::shared_ptr<ValueSubject<bool> > textLCDRendering;
            std::shared_ptr<Font::FontSystem> fontSystem;
            std::shared_ptr<Render> render;
        };

        void Render2DSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            ISystem::_init("djv::Render2D::Render2DSystem", context);

            DJV_PRIVATE_PTR();
            p.alphaBlend = ValueSubject<AlphaBlend>::create(AlphaBlend::Premultiplied);
            p.imageFilterOptions = ValueSubject<ImageFilterOptions>::create();
            p.textLCDRendering = ValueSubject<bool>::create(true);

            p.fontSystem = Font::FontSystem::create(context);
            p.render = Render2D::Render::create(context);

            addDependency(p.fontSystem);
            addDependency(p.render);
        }

        Render2DSystem::Render2DSystem() :
            _p(new Private)
        {}

        Render2DSystem::~Render2DSystem()
        {}

        std::shared_ptr<Render2DSystem> Render2DSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<Render2DSystem>(new Render2DSystem);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<AlphaBlend> > Render2DSystem::observeAlphaBlend() const
        {
            return _p->alphaBlend;
        }

        void Render2DSystem::setAlphaBlend(AlphaBlend value)
        {
            _p->alphaBlend->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<ImageFilterOptions> > Render2DSystem::observeImageFilterOptions() const
        {
            return _p->imageFilterOptions;
        }

        void Render2DSystem::setImageFilterOptions(const ImageFilterOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageFilterOptions->setIfChanged(value))
            {
                p.render->setImageFilterOptions(value);
            }
        }

        std::shared_ptr<IValueSubject<bool> > Render2DSystem::observeTextLCDRendering() const
        {
            return _p->textLCDRendering;
        }

        void Render2DSystem::setTextLCDRendering(bool value)
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

