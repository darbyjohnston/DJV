// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvRender3D/RenderSystem.h>

#include <djvRender3D/Render.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace Render3D
    {
        struct RenderSystem::Private
        {
        };

        void RenderSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            ISystem::_init("djv::Render3D::RenderSystem", context);
            DJV_PRIVATE_PTR();

            auto render = Render3D::Render::create(context);
            addDependency(render);
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

    } // namespace Render3D
} // namespace djv

