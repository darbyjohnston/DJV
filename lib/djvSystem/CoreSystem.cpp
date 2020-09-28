// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/CoreSystem.h>

#include <djvSystem/Animation.h>
#include <djvSystem/Context.h>
#include <djvSystem/Timer.h>

namespace djv
{
    namespace System
    {
        struct CoreSystem::Private
        {
        };

        void CoreSystem::_init(const std::string&, const std::shared_ptr<Context>& context)
        {
            ISystem::_init("djv::System::CoreSystem", context);

            auto animationSystem = Animation::AnimationSystem::create(context);
            addDependency(animationSystem);
        }

        CoreSystem::CoreSystem() :
            _p(new Private)
        {}

        CoreSystem::~CoreSystem()
        {}

        std::shared_ptr<CoreSystem> CoreSystem::create(const std::string& argv0, const std::shared_ptr<Context>& context)
        {
            auto out = context->getSystemT<CoreSystem>();
            if (!out)
            {
                out = std::shared_ptr<CoreSystem>(new CoreSystem);
                out->_init(argv0, context);
            }
            return out;
        }

    } // namespace System
} // namespace djv

