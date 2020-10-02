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
    namespace UIComponents
    {
        namespace Settings
        {
            struct FileBrowserWidget::Private
            {
                std::shared_ptr<ShortcutsWidget> widget;

                std::shared_ptr<Observer::Map<std::string, UI::ShortcutDataPair> > shortcutsObserver;
            };

            void FileBrowserWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                IWidget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UIComponents::Settings::FileBrowserWidget");

                p.widget = ShortcutsWidget::create(context);
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
                auto weak = std::weak_ptr<FileBrowserWidget>(std::dynamic_pointer_cast<FileBrowserWidget>(shared_from_this()));
                p.shortcutsObserver = Observer::Map<std::string, UI::ShortcutDataPair>::create(
                    fileBrowserSettings->observeKeyShortcuts(),
                    [weak](const std::map<std::string, UI::ShortcutDataPair>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->widget->setShortcuts(value);
                        }
                    });
            }

            FileBrowserWidget::FileBrowserWidget() :
                _p(new Private)
            {}

            std::shared_ptr<FileBrowserWidget> FileBrowserWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<FileBrowserWidget>(new FileBrowserWidget);
                out->_init(context);
                return out;
            }

            std::string FileBrowserWidget::getSettingsName() const
            {
                return DJV_TEXT("settings_file_browser_section_shortcuts");
            }

            std::string FileBrowserWidget::getSettingsGroup() const
            {
                return DJV_TEXT("settings_title_file_browser");
            }

            std::string FileBrowserWidget::getSettingsSortKey() const
            {
                return "b";
            }

        } // namespace FileBrowser
    } // namespace UIComponents
} // namespace djv
