// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene/SceneSystem.h>

#include <djvScene/IO.h>

#include <djvRender3D/Render.h>

#include <djvRender2D/Render2DSystem.h>

#include <djvAV/AVSystem.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene
    {
        struct SceneSystem::Private
        {
        };

        void SceneSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            ISystem::_init("djv::Scene::SceneSystem", context);
            addDependency(AV::AVSystem::create(context));
            addDependency(Render2D::Render2DSystem::create(context));
            addDependency(Render3D::Render::create(context));
            addDependency(IO::IOSystem::create(context));
        }

        SceneSystem::SceneSystem() :
            _p(new Private)
        {}

        SceneSystem::~SceneSystem()
        {}

        std::shared_ptr<SceneSystem> SceneSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<SceneSystem>(new SceneSystem);
            out->_init(context);
            return out;
        }

    } // namespace AV
} // namespace djv

