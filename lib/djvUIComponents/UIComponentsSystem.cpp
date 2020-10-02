// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/UIComponentsSystem.h>

#include <djvUIComponents/FileBrowserSettings.h>
#include <djvUIComponents/IOSettings.h>

#include <djvUI/UISystem.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        struct UIComponentsSystem::Private
        {};

        void UIComponentsSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            ISystem::_init("djv::UI::UIComponentsSystem", context);

            addDependency(context->getSystemT<UI::UISystem>());
            Settings::IO::create(context);
            Settings::FileBrowser::create(context);
        }

        UIComponentsSystem::UIComponentsSystem() :
            _p(new Private)
        {}

        UIComponentsSystem::~UIComponentsSystem()
        {}

        std::shared_ptr<UIComponentsSystem> UIComponentsSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<UIComponentsSystem>();
            if (!out)
            {
                out = std::shared_ptr<UIComponentsSystem>(new UIComponentsSystem);
                out->_init(context);
            }
            return out;
        }

    } // namespace UIComponents
} // namespace djv

