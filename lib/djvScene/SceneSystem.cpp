// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene/SceneSystem.h>

#include <djvScene/IO.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene
    {
        struct SceneSystem::Private
        {
        };

        void SceneSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISystem::_init("djv::Scene::AVSystem", context);
            addDependency(context->getSystemT<AV::AVSystem>());
            addDependency(IO::System::create(context));
        }

        SceneSystem::SceneSystem() :
            _p(new Private)
        {}

        SceneSystem::~SceneSystem()
        {}

        std::shared_ptr<SceneSystem> SceneSystem::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<SceneSystem>(new SceneSystem);
            out->_init(context);
            return out;
        }

    } // namespace AV
} // namespace djv

