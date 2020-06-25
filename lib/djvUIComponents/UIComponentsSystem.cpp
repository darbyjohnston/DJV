// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/UIComponentsSystem.h>

#include <djvUIComponents/FileBrowserSettings.h>
#include <djvUIComponents/IOSettings.h>

#include <djvUI/UISystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct UIComponentsSystem::Private
        {};

        void UIComponentsSystem::_init(const std::shared_ptr<Context>& context)
        {
            ISystem::_init("djv::UI::UIComponentsSystem", context);

            Settings::IO::create(context);
            Settings::FileBrowser::create(context);

            addDependency(context->getSystemT<UISystem>());
        }

        UIComponentsSystem::UIComponentsSystem() :
            _p(new Private)
        {}

        UIComponentsSystem::~UIComponentsSystem()
        {}

        std::shared_ptr<UIComponentsSystem> UIComponentsSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<UIComponentsSystem>(new UIComponentsSystem);
            out->_init(context);
            return out;
        }

    } // namespace UI
} // namespace djv

