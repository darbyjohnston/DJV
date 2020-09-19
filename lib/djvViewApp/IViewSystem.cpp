// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/IViewSystem.h>

#include <djvViewApp/KeyboardSettings.h>
#include <djvViewApp/MDIWidget.h>

#include <djvUIComponents/UIComponentsSystem.h>

#include <djvUI/MDICanvas.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct IViewSystem::Private
        {
            std::shared_ptr<UI::Settings::SettingsSystem> settingsSystem;
            std::shared_ptr<KeyboardSettings> keyboardSettings;

            std::shared_ptr<UI::MDI::Canvas> canvas;
            std::map<std::string, std::shared_ptr<MDIWidget> > widgets;
            std::map<std::string, Math::BBox2f> widgetGeom;

            std::shared_ptr<ValueObserver<bool> > textChangedObserver;
            std::shared_ptr<MapObserver<std::string, UI::ShortcutDataPair> > shortcutsObserver;
        };

        void IViewSystem::_init(const std::string& name, const std::shared_ptr<System::Context>& context)
        {
            ISystem::_init(name, context);
            DJV_PRIVATE_PTR();

            addDependency(context->getSystemT<UI::UIComponentsSystem>());

            p.settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            p.keyboardSettings = p.settingsSystem->getSettingsT<KeyboardSettings>();

            auto weak = std::weak_ptr<IViewSystem>(std::dynamic_pointer_cast<IViewSystem>(shared_from_this()));
            p.textChangedObserver = ValueObserver<bool>::create(
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
                p.shortcutsObserver = MapObserver<std::string, UI::ShortcutDataPair>::create(
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
            
        IViewSystem::IViewSystem() :
            _p(new Private)
        {}

        IViewSystem::~IViewSystem()
        {}
        
        std::map<std::string, std::shared_ptr<UI::Action> > IViewSystem::getActions() const
        {
            return std::map<std::string, std::shared_ptr<UI::Action> >();
        }

        MenuData IViewSystem::getMenu() const
        {
            return MenuData();
        }

        std::vector<std::shared_ptr<UI::ISettingsWidget> > IViewSystem::createSettingsWidgets() const
        {
            return std::vector<std::shared_ptr<UI::ISettingsWidget> >();
        }

        void IViewSystem::setCanvas(const std::shared_ptr<UI::MDI::Canvas>& value)
        {
            _p->canvas = value;
        }

        void IViewSystem::_openWidget(const std::string& name, const std::shared_ptr<MDIWidget>& widget)
        {
            DJV_PRIVATE_PTR();
            p.canvas->addChild(widget);
            p.widgets[name] = widget;
            auto weak = std::weak_ptr<IViewSystem>(std::dynamic_pointer_cast<IViewSystem>(shared_from_this()));
            widget->setCloseCallback(
                [weak, name]
                {
                    if (auto system = weak.lock())
                    {
                        system->_closeWidget(name);
                    }
                });
            const auto j = p.widgetGeom.find(name);
            if (j != p.widgetGeom.end())
            {
                p.canvas->setWidgetPos(widget, j->second.min);
                widget->resize(j->second.getSize());
            }
        }

        void IViewSystem::_closeWidget(const std::string& name)
        {
            DJV_PRIVATE_PTR();
            const auto i = p.widgets.find(name);
            if (i != p.widgets.end())
            {
                const glm::vec2& pos = p.canvas->getWidgetPos(i->second);
                const glm::vec2& size = i->second->getSize();
                p.widgetGeom[name] = Math::BBox2f(pos.x, pos.y, size.x, size.y);
                p.canvas->removeChild(i->second);
                p.widgets.erase(i);
            }
        }

        const std::map<std::string, std::shared_ptr<MDIWidget> >& IViewSystem::_getWidgets() const
        {
            return _p->widgets;
        }

        const std::map<std::string, Math::BBox2f>& IViewSystem::_getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void IViewSystem::_setWidgetGeom(const std::map<std::string, Math::BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        UI::ShortcutDataPair IViewSystem::_getShortcuts(const std::string& value) const
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

        void IViewSystem::_addShortcut(const std::string& name, const UI::ShortcutDataPair& value)
        {
            DJV_PRIVATE_PTR();
            if (p.keyboardSettings)
            {
                p.keyboardSettings->addShortcut(name, value);
            }
        }

        void IViewSystem::_addShortcut(const std::string& name, int key)
        {
            DJV_PRIVATE_PTR();
            if (p.keyboardSettings)
            {
                p.keyboardSettings->addShortcut(name, key);
            }
        }

        void IViewSystem::_addShortcut(const std::string& name, int key, int keyModifiers)
        {
            DJV_PRIVATE_PTR();
            if (p.keyboardSettings)
            {
                p.keyboardSettings->addShortcut(name, key, keyModifiers);
            }
        }

        void IViewSystem::_textUpdate()
        {}

        void IViewSystem::_shortcutsUpdate()
        {}

    } // namespace ViewApp
} // namespace djv
