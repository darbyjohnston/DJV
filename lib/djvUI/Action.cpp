// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Action.h>

#include <djvUI/Shortcut.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Action::Private
        {
            std::shared_ptr<ValueSubject<ButtonType> > buttonType;
            std::shared_ptr<ValueSubject<bool> > clicked;
            std::shared_ptr<ValueSubject<bool> > checked;
            std::string icon;
            std::string checkedIcon;
            std::shared_ptr<ValueSubject<std::string> > iconSubject;
            std::shared_ptr<ValueSubject<std::string> > text;
            std::shared_ptr<ValueSubject<std::string> > font;
            std::shared_ptr<ListSubject<std::shared_ptr<Shortcut> > > shortcuts;
            std::shared_ptr<ValueSubject<bool> > enabled;
            std::shared_ptr<ValueSubject<std::string> > tooltip;
            std::shared_ptr<ValueSubject<bool> > autoRepeat;
        };

        void Action::_init()
        {
            DJV_PRIVATE_PTR();
            p.buttonType = ValueSubject<ButtonType>::create(ButtonType::Push);
            p.clicked = ValueSubject<bool>::create(false);
            p.checked = ValueSubject<bool>::create(false);
            p.iconSubject = ValueSubject<std::string>::create();
            p.text = ValueSubject<std::string>::create();
            p.font = ValueSubject<std::string>::create();
            p.shortcuts = ListSubject<std::shared_ptr<Shortcut> >::create();
            p.enabled = ValueSubject<bool>::create(true);
            p.tooltip = ValueSubject<std::string>::create();
            p.autoRepeat = ValueSubject<bool>::create();
            _iconUpdate();
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

        std::shared_ptr<IValueSubject<ButtonType> > Action::observeButtonType() const
        {
            return _p->buttonType;
        }

        void Action::setButtonType(ButtonType value)
        {
            _p->buttonType->setIfChanged(value);
        }

        std::shared_ptr<Core::IValueSubject<bool> > Action::observeClicked() const
        {
            return _p->clicked;
        }

        void Action::doClicked()
        {
            _p->clicked->setAlways(true);
        }

        std::shared_ptr<IValueSubject<bool> > Action::observeChecked() const
        {
            return _p->checked;
        }

        void Action::setChecked(bool value)
        {
            if (_p->checked->setIfChanged(value))
            {
                _iconUpdate();
            }
        }

        std::shared_ptr<IValueSubject<std::string> > Action::observeIcon() const
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

        std::shared_ptr<IValueSubject<std::string> > Action::observeText() const
        {
            return _p->text;
        }

        void Action::setText(const std::string & value)
        {
            _p->text->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<std::string> > Action::observeFont() const
        {
            return _p->font;
        }

        void Action::setFont(const std::string & value)
        {
            _p->font->setIfChanged(value);
        }

        std::shared_ptr<Core::IListSubject<std::shared_ptr<Shortcut> > > Action::observeShortcuts() const
        {
            return _p->shortcuts;
        }

        void Action::setShortcut(const std::shared_ptr<Shortcut> & value)
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

        void Action::addShortcut(const std::shared_ptr<Shortcut> & value)
        {
            _p->shortcuts->pushBack(value);
            auto weak = std::weak_ptr<Action>(shared_from_this());
            value->setCallback(
                [weak]
            {
                if (auto action = weak.lock())
                {
                    switch (action->_p->buttonType->get())
                    {
                    case ButtonType::Push:
                        action->doClicked();
                        break;
                    case ButtonType::Toggle:
                        action->setChecked(!action->_p->checked->get());
                        break;
                    case ButtonType::Radio:
                        action->setChecked(true);
                        break;
                    case ButtonType::Exclusive:
                        action->setChecked(!action->_p->checked->get());
                        break;
                    default: break;
                    }
                }
            });
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
            auto shortcuts = p.shortcuts->get();
            for (auto & i : shortcuts)
            {
                i->setCallback(nullptr);
            }
            p.shortcuts->clear();
        }

        std::shared_ptr<Core::IValueSubject<bool> > Action::observeEnabled() const
        {
            return _p->enabled;
        }

        void Action::setEnabled(bool value)
        {
            _p->enabled->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<std::string> > Action::observeTooltip() const
        {
            return _p->tooltip;
        }

        void Action::setTooltip(const std::string& value)
        {
            _p->tooltip->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<bool> > Action::observeAutoRepeat() const
        {
            return _p->autoRepeat;
        }

        void Action::setAutoRepeat(bool value)
        {
            _p->autoRepeat->setIfChanged(value);
        }

        void Action::_iconUpdate()
        {
            if (_p->checked->get() && !_p->checkedIcon.empty())
            {
                _p->iconSubject->setIfChanged(_p->checkedIcon);
            }
            else
            {
                _p->iconSubject->setIfChanged(_p->icon);
            }
        }

    } // namespace UI
} // namespace djv
