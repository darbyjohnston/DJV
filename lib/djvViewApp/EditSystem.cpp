// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/EditSystem.h>

#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/ShortcutDataFunc.h>

#include <djvSystem/Context.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct EditSystem::Private
        {
            bool hasUndo = false;
            bool hasRedo = false;

            std::shared_ptr<MediaWidget> activeWidget;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;

            std::shared_ptr<Observer::Value<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<Observer::Value<bool> > hasUndoObserver;
            std::shared_ptr<Observer::Value<bool> > hasRedoObserver;
        };

        void EditSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewAppSystem::_init("djv::ViewApp::EditSystem", context);
            DJV_PRIVATE_PTR();

            p.actions["Undo"] = UI::Action::create();
            p.actions["Redo"] = UI::Action::create();

            _addShortcut(DJV_TEXT("shortcut_undo"), GLFW_KEY_Z, UI::getSystemModifier());
            _addShortcut(DJV_TEXT("shortcut_redo"), GLFW_KEY_Z, UI::getSystemModifier() | GLFW_MOD_SHIFT);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Undo"]);
            p.menu->addAction(p.actions["Redo"]);

            _actionsUpdate();
            _textUpdate();
            _shortcutsUpdate();
            
            auto weak = std::weak_ptr<EditSystem>(std::dynamic_pointer_cast<EditSystem>(shared_from_this()));
            p.actions["Undo"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto widget = system->_p->activeWidget)
                        {
                            widget->getMedia()->undo();
                        }
                    }
                });
                
            p.actions["Redo"]->setClickedCallback(
                [weak]
                {
                    if (auto system = weak.lock())
                    {
                        if (auto widget = system->_p->activeWidget)
                        {
                            widget->getMedia()->redo();
                        }
                    }
                });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.activeWidgetObserver = Observer::Value<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak, contextWeak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->activeWidget = value;
                            if (system->_p->activeWidget)
                            {
                                system->_p->hasUndoObserver = Observer::Value<bool>::create(
                                    system->_p->activeWidget->getMedia()->observeHasUndo(),
                                    [weak](bool value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->hasUndo = value;
                                            system->_actionsUpdate();
                                        }
                                    });

                                system->_p->hasRedoObserver = Observer::Value<bool>::create(
                                    system->_p->activeWidget->getMedia()->observeHasRedo(),
                                    [weak](bool value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->hasRedo = value;
                                            system->_actionsUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                system->_p->hasUndo = false;
                                system->_p->hasRedo = false;
                                system->_p->hasUndoObserver.reset();
                                system->_p->hasRedoObserver.reset();
                                system->_actionsUpdate();
                            }
                        }
                    });
            }

            _logInitTime();
        }

        EditSystem::EditSystem() :
            _p(new Private)
        {}

        EditSystem::~EditSystem()
        {}

        std::shared_ptr<EditSystem> EditSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<EditSystem>();
            if (!out)
            {
                out = std::shared_ptr<EditSystem>(new EditSystem);
                out->_init(context);
            }
            return out;
        }

        int EditSystem::getSortKey() const
        {
            return 2;
        }

        std::map<std::string, std::shared_ptr<UI::Action>> EditSystem::getActions() const
        {
            return _p->actions;
        }

        std::vector<std::shared_ptr<UI::Menu> > EditSystem::getMenus() const
        {
            return { _p->menu };
        }

        void EditSystem::_actionsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Undo"]->setEnabled(p.hasUndo);
                p.actions["Redo"]->setEnabled(p.hasRedo);
            }
        }

        void EditSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Undo"]->setText(_getText(DJV_TEXT("menu_edit_undo")));
                p.actions["Redo"]->setText(_getText(DJV_TEXT("menu_edit_redo")));

                p.menu->setText(_getText(DJV_TEXT("menu_edit")));
            }
        }

        void EditSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Undo"]->setShortcuts(_getShortcuts("shortcut_undo"));
                p.actions["Redo"]->setShortcuts(_getShortcuts("shortcut_redo"));
            }
        }

    } // namespace ViewApp
} // namespace djv

