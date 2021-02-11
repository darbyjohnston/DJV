// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/IViewAppSystem.h>

#include <djvViewApp/KeyboardSettings.h>

#include <djvUIComponents/UIComponentsSystem.h>

#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/Widget.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        ToolWidgetData::ToolWidgetData()
        {}
        
        ToolWidgetData::ToolWidgetData(
            const std::shared_ptr<UI::Widget>& titleBar,
            const std::shared_ptr<UI::Widget>& toolBar,
            const std::shared_ptr<UI::Widget>& widget,
            const std::shared_ptr<UI::Widget>& footer) :
            titleBar(titleBar),
            toolBar(toolBar),
            widget(widget),
            footer(footer)
        {}

        struct IViewAppSystem::Private
        {
            std::shared_ptr<UI::Settings::SettingsSystem> settingsSystem;
            std::shared_ptr<KeyboardSettings> keyboardSettings;

            std::shared_ptr<Observer::Value<bool> > textChangedObserver;
            std::shared_ptr<Observer::Map<std::string, UI::ShortcutDataPair> > shortcutsObserver;
        };

        void IViewAppSystem::_init(const std::string& name, const std::shared_ptr<System::Context>& context)
        {
            ISystem::_init(name, context);
            DJV_PRIVATE_PTR();

            addDependency(UIComponents::UIComponentsSystem::create(context));

            p.settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            p.keyboardSettings = p.settingsSystem->getSettingsT<KeyboardSettings>();

            auto weak = std::weak_ptr<IViewAppSystem>(std::dynamic_pointer_cast<IViewAppSystem>(shared_from_this()));
            p.textChangedObserver = Observer::Value<bool>::create(
                context->getSystemT<System::TextSystem>()->observeTextChanged(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_textUpdate();
                        }
                    }
                });

            if (p.keyboardSettings)
            {
                p.shortcutsObserver = Observer::Map<std::string, UI::ShortcutDataPair>::create(
                    p.keyboardSettings->observeShortcuts(),
                    [weak](const std::map<std::string, UI::ShortcutDataPair>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_shortcutsUpdate();
                        }
                    });
            }
        }
            
        IViewAppSystem::IViewAppSystem() :
            _p(new Private)
        {}

        IViewAppSystem::~IViewAppSystem()
        {}
        
        std::map<std::string, std::shared_ptr<UI::Action> > IViewAppSystem::getActions() const
        {
            return {};
        }

        std::vector<std::shared_ptr<UI::Menu> > IViewAppSystem::getMenus() const
        {
            return {};
        }

        std::vector<std::shared_ptr<UI::ToolBar> > IViewAppSystem::createToolBars() const
        {
            return {};
        }

        std::vector<std::shared_ptr<UI::Action> > IViewAppSystem::getToolWidgetActions() const
        {
            return {};
        }

        std::vector<std::shared_ptr<UI::Action> > IViewAppSystem::getToolWidgetToolBarActions() const
        {
            return {};
        }

        ToolWidgetData IViewAppSystem::createToolWidget(const std::shared_ptr<UI::Action>&)
        {
            return ToolWidgetData();
        }

        void IViewAppSystem::deleteToolWidget(const std::shared_ptr<UI::Action>&)
        {}

        std::vector<std::shared_ptr<UIComponents::Settings::IWidget> > IViewAppSystem::createSettingsWidgets() const
        {
            return std::vector<std::shared_ptr<UIComponents::Settings::IWidget> >();
        }

        UI::ShortcutDataPair IViewAppSystem::_getShortcuts(const std::string& value) const
        {
            DJV_PRIVATE_PTR();
            UI::ShortcutDataMap shortcuts;
            if (p.keyboardSettings)
            {
                shortcuts = p.keyboardSettings->observeShortcuts()->get();
            }
            const auto i = shortcuts.find(value);
            return i != shortcuts.end() ? i->second : UI::ShortcutDataPair();
        }

        void IViewAppSystem::_addShortcut(const std::string& name, const UI::ShortcutDataPair& value)
        {
            DJV_PRIVATE_PTR();
            if (p.keyboardSettings)
            {
                p.keyboardSettings->addShortcut(name, value);
            }
        }

        void IViewAppSystem::_addShortcut(const std::string& name, int key)
        {
            DJV_PRIVATE_PTR();
            if (p.keyboardSettings)
            {
                p.keyboardSettings->addShortcut(name, key);
            }
        }

        void IViewAppSystem::_addShortcut(const std::string& name, int key, int keyModifiers)
        {
            DJV_PRIVATE_PTR();
            if (p.keyboardSettings)
            {
                p.keyboardSettings->addShortcut(name, key, keyModifiers);
            }
        }

        void IViewAppSystem::_textUpdate()
        {}

        void IViewAppSystem::_shortcutsUpdate()
        {}

    } // namespace ViewApp
} // namespace djv
