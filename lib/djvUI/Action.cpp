// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Action.h>

#include <djvUI/Shortcut.h>
#include <djvUI/ShortcutData.h>

#include <iostream>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Action::Private
        {
            std::shared_ptr<Observer::ValueSubject<ButtonType> > buttonTypeSubject;
            std::shared_ptr<Observer::ValueSubject<bool> > checkedSubject;
            std::string icon;
            std::string checkedIcon;
            std::shared_ptr<Observer::ValueSubject<std::string> > iconSubject;
            std::shared_ptr<Observer::ValueSubject<std::string> > textSubject;
            std::shared_ptr<Observer::ValueSubject<std::string> > fontSubject;
            std::shared_ptr<Observer::ListSubject<std::shared_ptr<Shortcut> > > shortcutsSubject;
            std::shared_ptr<Observer::ValueSubject<bool> > enabledSubject;
            std::shared_ptr<Observer::ValueSubject<std::string> > tooltipSubject;
            std::shared_ptr<Observer::ValueSubject<bool> > autoRepeatSubject;
            std::function<void(void)> clickedCallback;
            std::function<void(bool)> checkedCallback;
        };

        void Action::_init()
        {
            DJV_PRIVATE_PTR();
            p.buttonTypeSubject = Observer::ValueSubject<ButtonType>::create(ButtonType::Push);
            p.checkedSubject = Observer::ValueSubject<bool>::create(false);
            p.iconSubject = Observer::ValueSubject<std::string>::create();
            p.textSubject = Observer::ValueSubject<std::string>::create();
            p.fontSubject = Observer::ValueSubject<std::string>::create();
            p.shortcutsSubject = Observer::ListSubject<std::shared_ptr<Shortcut> >::create();
            p.enabledSubject = Observer::ValueSubject<bool>::create(true);
            p.tooltipSubject = Observer::ValueSubject<std::string>::create();
            p.autoRepeatSubject = Observer::ValueSubject<bool>::create();
        }

        Action::Action() :
            _p(new Private)
        {}

        Action::~Action()
        {}

        std::shared_ptr<Action> Action::create()
        {
            auto out = std::shared_ptr<Action>(new Action);
            out->_init();
            return out;
        }

        std::shared_ptr<Action> Action::create(const std::string& text)
        {
            auto out = create();
            out->setText(text);
            return out;
        }

        std::shared_ptr<Observer::IValueSubject<ButtonType> > Action::observeButtonType() const
        {
            return _p->buttonTypeSubject;
        }

        void Action::setButtonType(ButtonType value)
        {
            _p->buttonTypeSubject->setIfChanged(value);
        }

        std::shared_ptr<Observer::IValueSubject<bool> > Action::observeChecked() const
        {
            return _p->checkedSubject;
        }

        void Action::setChecked(bool value)
        {
            if (_p->checkedSubject->setIfChanged(value))
            {
                _iconUpdate();
            }
        }

        std::shared_ptr<Observer::IValueSubject<std::string> > Action::observeIcon() const
        {
            return _p->iconSubject;
        }

        void Action::setIcon(const std::string& value)
        {
            if (value == _p->icon)
                return;
            _p->icon = value;
            _iconUpdate();
        }

        void Action::setCheckedIcon(const std::string& value)
        {
            if (value == _p->checkedIcon)
                return;
            _p->checkedIcon = value;
            _iconUpdate();
        }

        std::shared_ptr<Observer::IValueSubject<std::string> > Action::observeText() const
        {
            return _p->textSubject;
        }

        void Action::setText(const std::string& value)
        {
            _p->textSubject->setIfChanged(value);
        }

        std::shared_ptr<Observer::IValueSubject<std::string> > Action::observeFont() const
        {
            return _p->fontSubject;
        }

        void Action::setFont(const std::string& value)
        {
            _p->fontSubject->setIfChanged(value);
        }

        std::shared_ptr<Core::Observer::IListSubject<std::shared_ptr<Shortcut> > > Action::observeShortcuts() const
        {
            return _p->shortcutsSubject;
        }

        void Action::setShortcuts(const ShortcutDataPair& value)
        {
            clearShortcuts();
            addShortcut(value.primary);
            addShortcut(value.secondary);
        }

        void Action::setShortcut(const std::shared_ptr<Shortcut>& value)
        {
            clearShortcuts();
            addShortcut(value);
        }

        void Action::setShortcut(int key)
        {
            setShortcut(UI::Shortcut::create(key));
        }

        void Action::setShortcut(int key, int keyModifiers)
        {
            setShortcut(UI::Shortcut::create(key, keyModifiers));
        }

        void Action::addShortcut(const std::shared_ptr<Shortcut>& value)
        {
            _p->shortcutsSubject->pushBack(value);
            auto weak = std::weak_ptr<Action>(shared_from_this());
            value->setCallback(
                [weak]
            {
                if (auto action = weak.lock())
                {
                    action->doClick();
                }
            });
        }

        void Action::addShortcut(const ShortcutData& value)
        {
            addShortcut(UI::Shortcut::create(value));
        }

        void Action::addShortcut(int key)
        {
            addShortcut(UI::Shortcut::create(key));
        }

        void Action::addShortcut(int key, int keyModifiers)
        {
            addShortcut(UI::Shortcut::create(key, keyModifiers));
        }

        void Action::clearShortcuts()
        {
            DJV_PRIVATE_PTR();
            const auto shortcuts = p.shortcutsSubject->get();
            for (auto& i : shortcuts)
            {
                i->setCallback(nullptr);
            }
            p.shortcutsSubject->clear();
        }

        std::shared_ptr<Core::Observer::IValueSubject<bool> > Action::observeEnabled() const
        {
            return _p->enabledSubject;
        }

        void Action::setEnabled(bool value)
        {
            _p->enabledSubject->setIfChanged(value);
        }

        std::shared_ptr<Observer::IValueSubject<std::string> > Action::observeTooltip() const
        {
            return _p->tooltipSubject;
        }

        void Action::setTooltip(const std::string& value)
        {
            _p->tooltipSubject->setIfChanged(value);
        }

        std::shared_ptr<Observer::IValueSubject<bool> > Action::observeAutoRepeat() const
        {
            return _p->autoRepeatSubject;
        }

        void Action::setAutoRepeat(bool value)
        {
            _p->autoRepeatSubject->setIfChanged(value);
        }

        void Action::setClickedCallback(const std::function<void(void)>& callback)
        {
            _p->clickedCallback = callback;
        }

        void Action::setCheckedCallback(const std::function<void(bool)>& callback)
        {
            _p->checkedCallback = callback;
        }

        void Action::doClick()
        {
            DJV_PRIVATE_PTR();
            if (p.clickedCallback)
            {
                p.clickedCallback();
            }
            switch (p.buttonTypeSubject->get())
            {
            case ButtonType::Toggle:
                if (p.checkedSubject->setIfChanged(!p.checkedSubject->get()))
                {
                    _iconUpdate();
                    if (p.checkedCallback)
                    {
                        p.checkedCallback(p.checkedSubject->get());
                    }
                }
                break;
            case ButtonType::Radio:
                if (p.checkedSubject->setIfChanged(true))
                {
                    _iconUpdate();
                    if (p.checkedCallback)
                    {
                        p.checkedCallback(p.checkedSubject->get());
                    }
                }
                break;
            case ButtonType::Exclusive:
                if (p.checkedSubject->setIfChanged(!p.checkedSubject->get()))
                {
                    _iconUpdate();
                    if (p.checkedCallback)
                    {
                        p.checkedCallback(p.checkedSubject->get());
                    }
                }
                break;
            default: break;
            }
        }

        void Action::_iconUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.checkedSubject->get() && !p.checkedIcon.empty())
            {
                p.iconSubject->setIfChanged(p.checkedIcon);
            }
            else
            {
                p.iconSubject->setIfChanged(p.icon);
            }
        }

    } // namespace UI
} // namespace djv
