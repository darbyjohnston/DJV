//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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
            std::shared_ptr<ValueSubject<std::string> > icon;
            std::shared_ptr<ValueSubject<Style::MetricsRole> > iconSizeRole;
            std::shared_ptr<ValueSubject<std::string> > text;
            std::shared_ptr<ValueSubject<std::string> > fontFace;
            std::shared_ptr<ValueSubject<Style::MetricsRole> > fontSizeRole;
            std::shared_ptr<ListSubject<std::shared_ptr<Shortcut> > > shortcuts;
            std::shared_ptr<ValueSubject<bool> > enabled;
            std::shared_ptr<ValueSubject<std::string> > tooltip;
        };

        void Action::_init()
        {
            _p->buttonType = ValueSubject<ButtonType>::create(ButtonType::Push);
            _p->clicked = ValueSubject<bool>::create(false);
            _p->checked = ValueSubject<bool>::create(false);
            _p->icon = ValueSubject<std::string>::create();
            _p->iconSizeRole = ValueSubject<Style::MetricsRole>::create(Style::MetricsRole::Icon);
            _p->text = ValueSubject<std::string>::create();
            _p->fontFace = ValueSubject<std::string>::create(AV::Font::Info::defaultFace);
            _p->fontSizeRole = ValueSubject<Style::MetricsRole>::create(Style::MetricsRole::FontMedium);
            _p->shortcuts = ListSubject<std::shared_ptr<Shortcut> >::create();
            _p->enabled = ValueSubject<bool>::create(true);
            _p->tooltip = ValueSubject<std::string>::create();
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

        std::shared_ptr<IValueSubject<ButtonType> > Action::getButtonType() const
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

        std::shared_ptr<IValueSubject<bool> > Action::isChecked() const
        {
            return _p->checked;
        }

        void Action::setChecked(bool value)
        {
            _p->checked->setIfChanged(value);
        }

        void Action::doChecked()
        {
            const bool value = _p->checked->get();
            _p->checked->setAlways(value);
        }

        std::shared_ptr<IValueSubject<std::string> > Action::getIcon() const
        {
            return _p->icon;
        }

        std::shared_ptr<IValueSubject<Style::MetricsRole> > Action::getIconSizeRole() const
        {
            return _p->iconSizeRole;
        }

        void Action::setIconSizeRole(Style::MetricsRole value)
        {
            _p->iconSizeRole->setIfChanged(value);
        }

        void Action::setIcon(const std::string& value)
        {
            _p->icon->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<std::string> > Action::getText() const
        {
            return _p->text;
        }

        void Action::setText(const std::string& value)
        {
            _p->text->setIfChanged(value);
        }

        std::shared_ptr<Core::IValueSubject<std::string> > Action::getFontFace() const
        {
            return _p->fontFace;
        }

        std::shared_ptr<Core::IValueSubject<Style::MetricsRole> > Action::getFontSizeRole() const
        {
            return _p->fontSizeRole;
        }

        void Action::setFontFace(const std::string & value)
        {
            _p->fontFace->setIfChanged(value);
        }

        void Action::setFontSizeRole(Style::MetricsRole value)
        {
            _p->fontSizeRole->setIfChanged(value);
        }

        std::shared_ptr<Core::IListSubject<std::shared_ptr<Shortcut> > > Action::getShortcuts() const
        {
            return _p->shortcuts;
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
                        action->doChecked();
                        break;
                    case ButtonType::Radio:
                        action->setChecked(true);
                        action->doChecked();
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
            auto shortcuts = _p->shortcuts->get();
            for (auto & i : shortcuts)
            {
                i->setCallback(nullptr);
            }
            _p->shortcuts->clear();
        }

        std::shared_ptr<Core::IValueSubject<bool> > Action::isEnabled() const
        {
            return _p->enabled;
        }

        void Action::setEnabled(bool value)
        {
            _p->enabled->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<std::string> > Action::getTooltip() const
        {
            return _p->tooltip;
        }

        void Action::setTooltip(const std::string& value)
        {
            _p->tooltip->setIfChanged(value);
        }

        struct ActionGroup::Private
        {
            std::vector<std::shared_ptr<Action> > actions;
            ButtonType buttonType = ButtonType::Push;
            std::function<void(int)> clickedCallback;
            std::function<void(int, bool)> checkedCallback;
            std::function<void(int)> radioCallback;
            std::map<std::shared_ptr<Action>, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::map<std::shared_ptr<Action>, std::shared_ptr<ValueObserver<bool> > > checkedObservers;
        };

        ActionGroup::ActionGroup() :
            _p(new Private)
        {}

        ActionGroup::~ActionGroup()
        {
            clearActions();
        }

        std::shared_ptr<ActionGroup> ActionGroup::create(ButtonType buttonType)
        {
            auto out = std::shared_ptr<ActionGroup>(new ActionGroup);
            out->setButtonType(buttonType);
            return out;
        }

        const std::vector<std::shared_ptr<Action> >& ActionGroup::getActions() const
        {
            return _p->actions;
        }

        size_t ActionGroup::getActionCount() const
        {
            return _p->actions.size();
        }

        int ActionGroup::getActionIndex(const std::shared_ptr<Action>& value) const
        {
            const auto i = std::find(_p->actions.begin(), _p->actions.end(), value);
            if (i != _p->actions.end())
            {
                return static_cast<int>(i - _p->actions.begin());
            }
            return -1;
        }

        void ActionGroup::addAction(const std::shared_ptr<Action>& action)
        {
            action->setButtonType(_p->buttonType);

            if (ButtonType::Radio == _p->buttonType)
            {
                size_t i = 0;
                for (; i < _p->actions.size(); ++i)
                {
                    if (_p->actions[i]->isChecked()->get())
                    {
                        break;
                    }
                }
                if (i == _p->actions.size())
                {
                    action->setChecked(true);
                }
            }

            const int index = static_cast<int>(_p->actions.size());

            auto weak = std::weak_ptr<ActionGroup>(std::dynamic_pointer_cast<ActionGroup>(shared_from_this()));
            _p->clickedObservers[action] =
                ValueObserver<bool>::create(
                    action->observeClicked(),
                    [weak, index](bool value)
            {
                if (value)
                {
                    if (auto group = weak.lock())
                    {
                        if (group->_p->clickedCallback)
                        {
                            group->_p->clickedCallback(index);
                        }
                    }
                }
            });

            _p->checkedObservers[action] =
                ValueObserver<bool>::create(
                    action->isChecked(),
                    [this, index](bool value)
            {
                if (value && ButtonType::Radio == _p->buttonType && _p->actions.size() > 0)
                {
                    for (size_t i = 0; i < _p->actions.size(); ++i)
                    {
                        _p->actions[i]->setChecked(i == index);
                    }
                    if (_p->radioCallback)
                    {
                        _p->radioCallback(index);
                    }
                }
                if (_p->checkedCallback)
                {
                    _p->checkedCallback(index, true);
                }
            });

            _p->actions.push_back(action);
        }

        void ActionGroup::removeAction(const std::shared_ptr<Action>& action)
        {
            const auto i = std::find(_p->actions.begin(), _p->actions.end(), action);
            if (i != _p->actions.end())
            {
                const auto j = _p->clickedObservers.find(*i);
                if (j != _p->clickedObservers.end())
                {
                    _p->clickedObservers.erase(j);
                }
                const auto k = _p->checkedObservers.find(*i);
                if (k != _p->checkedObservers.end())
                {
                    _p->checkedObservers.erase(k);
                }
                _p->actions.erase(i);
            }
        }

        void ActionGroup::clearActions()
        {
            _p->actions.clear();
            _p->clickedObservers.clear();
            _p->checkedObservers.clear();
        }

        ButtonType ActionGroup::getButtonType() const
        {
            return _p->buttonType;
        }

        void ActionGroup::setButtonType(ButtonType value)
        {
            _p->buttonType = value;

            for (size_t i = 0; i < _p->actions.size(); ++i)
            {
                _p->actions[i]->setButtonType(value);
            }

            if (ButtonType::Radio == _p->buttonType)
            {
                setChecked(0);
            }
        }

        void ActionGroup::setChecked(int index, bool value)
        {
            if (index >= 0 && index < static_cast<int>(_p->actions.size()))
            {
                if (_p->actions[index]->isChecked()->get() != value)
                {
                    if (ButtonType::Radio == _p->buttonType)
                    {
                        for (size_t i = 0; i < _p->actions.size(); ++i)
                        {
                            _p->actions[i]->setChecked(i == index);
                        }

                        if (_p->radioCallback)
                        {
                            _p->radioCallback(index);
                        }
                    }
                    else
                    {
                        _p->actions[index]->setChecked(value);
                    }

                    if (_p->checkedCallback)
                    {
                        _p->checkedCallback(index, value);
                    }
                }
            }
        }

        void ActionGroup::setClickedCallback(const std::function<void(int)>& callback)
        {
            _p->clickedCallback = callback;
        }

        void ActionGroup::setCheckedCallback(const std::function<void(int, bool)>& callback)
        {
            _p->checkedCallback = callback;
        }

        void ActionGroup::setRadioCallback(const std::function<void(int)>& callback)
        {
            _p->radioCallback = callback;
        }

    } // namespace UI
} // namespace djv
