// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/CoreSystem.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>
#include <djvCore/Timer.h>

using namespace djv::Core;

namespace djv
{
    namespace Core
    {
        struct CoreSystem::Private
        {
        };

        void CoreSystem::_init(const std::string&, const std::shared_ptr<Context>& context)
        {
            ISystem::_init("djv::Core::CoreSystem", context);

            auto animationSystem = Animation::System::create(context);

            addDependency(animationSystem);
        }

        CoreSystem::CoreSystem() :
            _p(new Private)
        {}

        CoreSystem::~CoreSystem()
        {}

        std::shared_ptr<CoreSystem> CoreSystem::create(const std::string& argv0, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<CoreSystem>(new CoreSystem);
            out->_init(argv0, context);
            return out;
        }

    } // namespace Core
} // namespace djv

