// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/UISystem.h>

#include <djvUI/AVSettings.h>
#include <djvUI/DialogSystem.h>
#include <djvUI/ColorSpaceSettings.h>
#include <djvUI/GeneralSettings.h>
#include <djvUI/FontSettings.h>
#include <djvUI/IconSystem.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Style.h>
#include <djvUI/StyleSettings.h>
#include <djvUI/UISettings.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct UISystem::Private
        {
            std::shared_ptr<Style::Style> style;
        };

        void UISystem::_init(bool resetSettings, const std::shared_ptr<Core::Context>& context)
        {
            ISystem::_init("djv::UI::UISystem", context);

            DJV_PRIVATE_PTR();

            addDependency(context->getSystemT<AV::AVSystem>());

            auto settingsSystem = Settings::System::create(resetSettings, context);
            Settings::AV::create(context);
            Settings::ColorSpace::create(context);
            Settings::General::create(context);
            Settings::Font::create(context);
            Settings::UI::create(context);
            Settings::Style::create(context);

            auto iconSystem = IconSystem::create(context);

            p.style = Style::Style::create(context);
            
            auto dialogSystem = DialogSystem::create(context);

            addDependency(settingsSystem);
            addDependency(iconSystem);
            addDependency(dialogSystem);
        }

        UISystem::UISystem() :
            _p(new Private)
        {}

        UISystem::~UISystem()
        {}

        std::shared_ptr<UISystem> UISystem::create(bool resetSettings, const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<UISystem>(new UISystem);
            out->_init(resetSettings, context);
            return out;
        }

        const std::shared_ptr<Style::Style>& UISystem::getStyle() const
        {
            return _p->style;
        }

    } // namespace UI
} // namespace djv

