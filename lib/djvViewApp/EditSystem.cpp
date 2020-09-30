// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/EditSystem.h>

#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/ShortcutData.h>

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
            std::shared_ptr<MediaWidget> activeWidget;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::shared_ptr<Observer::ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<Observer::ValueObserver<bool> > hasUndoObserver;
            std::shared_ptr<Observer::ValueObserver<bool> > hasRedoObserver;
        };

        void EditSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::EditSystem", context);
            DJV_PRIVATE_PTR();

            p.actions["Undo"] = UI::Action::create();
            p.actions["Redo"] = UI::Action::create();

            _addShortcut("shortcut_undo", GLFW_KEY_Z, UI::ShortcutData::getSystemModifier());
            _addShortcut("shortcut_redo", GLFW_KEY_Z, UI::ShortcutData::getSystemModifier() | GLFW_MOD_SHIFT);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Undo"]);
            p.menu->addAction(p.actions["Redo"]);

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
                p.activeWidgetObserver = Observer::ValueObserver<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak, contextWeak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->activeWidget = value;
                            if (system->_p->activeWidget)
                            {
                                system->_p->hasUndoObserver = Observer::ValueObserver<bool>::create(
                                    system->_p->activeWidget->getMedia()->observeHasUndo(),
                                    [weak](bool value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->actions["Undo"]->setEnabled(value);
                                        }
                                    });

                                system->_p->hasRedoObserver = Observer::ValueObserver<bool>::create(
                                    system->_p->activeWidget->getMedia()->observeHasRedo(),
                                    [weak](bool value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->actions["Redo"]->setEnabled(value);
                                        }
                                    });
                            }
                            else
                            {
                                system->_p->actions["Undo"]->setEnabled(false);
                                system->_p->actions["Redo"]->setEnabled(false);
                                system->_p->hasUndoObserver.reset();
                                system->_p->hasRedoObserver.reset();
                            }
                        }
                    });
            }
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

        std::map<std::string, std::shared_ptr<UI::Action> > EditSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData EditSystem::getMenu() const
        {
            return
            {
                _p->menu,
                "B"
            };
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

