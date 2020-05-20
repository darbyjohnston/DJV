// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/InputSystem.h>

#include <djvViewApp/InputSettings.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct InputSystem::Private
        {
            std::shared_ptr<InputSettings> settings;
        };

        void InputSystem::_init(const std::shared_ptr<Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::InputSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = InputSettings::create(context);

        }

        InputSystem::InputSystem() :
            _p(new Private)
        {}

        InputSystem::~InputSystem()
        {}

        std::shared_ptr<InputSystem> InputSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<InputSystem>(new InputSystem);
            out->_init(context);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

