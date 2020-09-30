// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserSettingsWidget.h>

#include <djvUIComponents/FileBrowserSettings.h>
#include <djvUIComponents/ShortcutsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct SettingsWidget::Private
            {
                std::shared_ptr<UI::ShortcutsWidget> widget;

                std::shared_ptr<Observer::MapObserver<std::string, UI::ShortcutDataPair> > shortcutsObserver;
            };

            void SettingsWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                ISettingsWidget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::FileBrowser::SettingsWidget");

                p.widget = UI::ShortcutsWidget::create(context);
                addChild(p.widget);

                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.widget->setShortcutsCallback(
                    [contextWeak](const UI::ShortcutDataMap& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                            fileBrowserSettings->setKeyShortcuts(value);
                        }
                    });

                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                auto weak = std::weak_ptr<SettingsWidget>(std::dynamic_pointer_cast<SettingsWidget>(shared_from_this()));
                p.shortcutsObserver = Observer::MapObserver<std::string, UI::ShortcutDataPair>::create(
                    fileBrowserSettings->observeKeyShortcuts(),
                    [weak](const std::map<std::string, UI::ShortcutDataPair>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->widget->setShortcuts(value);
                        }
                    });
            }

            SettingsWidget::SettingsWidget() :
                _p(new Private)
            {}

            std::shared_ptr<SettingsWidget> SettingsWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<SettingsWidget>(new SettingsWidget);
                out->_init(context);
                return out;
            }

            std::string SettingsWidget::getSettingsName() const
            {
                return DJV_TEXT("settings_file_browser_section_shortcuts");
            }

            std::string SettingsWidget::getSettingsGroup() const
            {
                return DJV_TEXT("settings_title_file_browser");
            }

            std::string SettingsWidget::getSettingsSortKey() const
            {
                return "b";
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
