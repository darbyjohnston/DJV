// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/KeyboardSettingsWidget.h>

#include <djvViewApp/KeyboardSettings.h>

#include <djvUIComponents/ShortcutsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct KeyboardShortcutSettingsWidget::Private
        {
            std::shared_ptr<UI::ShortcutsWidget> widget;

            std::shared_ptr<Observer::Map<std::string, UI::ShortcutDataPair> > shortcutsObserver;
        };

        void KeyboardShortcutSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::KeyboardShortcutSettingsWidget");

            p.widget = UI::ShortcutsWidget::create(context);
            addChild(p.widget);

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.widget->setShortcutsCallback(
                [contextWeak](const UI::ShortcutDataMap& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        auto keyboardSettings = settingsSystem->getSettingsT<KeyboardSettings>();
                        keyboardSettings->setShortcuts(value);
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto keyboardSettings = settingsSystem->getSettingsT<KeyboardSettings>();
            auto weak = std::weak_ptr<KeyboardShortcutSettingsWidget>(std::dynamic_pointer_cast<KeyboardShortcutSettingsWidget>(shared_from_this()));
            p.shortcutsObserver = Observer::Map<std::string, UI::ShortcutDataPair>::create(
                keyboardSettings->observeShortcuts(),
                [weak](const std::map<std::string, UI::ShortcutDataPair>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->widget->setShortcuts(value);
                    }
                });
        }

        KeyboardShortcutSettingsWidget::KeyboardShortcutSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<KeyboardShortcutSettingsWidget> KeyboardShortcutSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<KeyboardShortcutSettingsWidget>(new KeyboardShortcutSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string KeyboardShortcutSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_keyboard_section_shortcuts");
        }

        std::string KeyboardShortcutSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_keyboard");
        }

        std::string KeyboardShortcutSettingsWidget::getSettingsSortKey() const
        {
            return "Y";
        }

    } // namespace ViewApp
} // namespace djv

