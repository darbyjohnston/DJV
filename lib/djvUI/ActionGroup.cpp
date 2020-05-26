// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ActionGroup.h>

#include <djvUI/Action.h>
#include <djvUI/Shortcut.h>

#include <map>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ActionGroup::Private
        {
            std::vector<std::shared_ptr<Action> > actions;
            ButtonType buttonType = ButtonType::Push;
            std::function<void(int)> pushCallback;
            std::function<void(int, bool)> toggleCallback;
            std::function<void(int)> radioCallback;
            std::function<void(int)> exclusiveCallback;
            Callback callback = Callback::Trigger;
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

        const std::vector<std::shared_ptr<Action> > & ActionGroup::getActions() const
        {
            return _p->actions;
        }

        size_t ActionGroup::getActionCount() const
        {
            return _p->actions.size();
        }

        int ActionGroup::getActionIndex(const std::shared_ptr<Action> & value) const
        {
            DJV_PRIVATE_PTR();
            const auto i = std::find(p.actions.begin(), p.actions.end(), value);
            if (i != p.actions.end())
            {
                return static_cast<int>(i - p.actions.begin());
            }
            return -1;
        }

        void ActionGroup::addAction(const std::shared_ptr<Action> & action)
        {
            DJV_PRIVATE_PTR();
            action->setButtonType(p.buttonType);

            auto weak = std::weak_ptr<ActionGroup>(std::dynamic_pointer_cast<ActionGroup>(shared_from_this()));
            const int index = static_cast<int>(p.actions.size());
            p.clickedObservers[action] = ValueObserver<bool>::create(
                action->observeClicked(),
                [weak, index](bool value)
            {
                if (value)
                {
                    if (auto group = weak.lock())
                    {
                        if (group->_p->pushCallback)
                        {
                            group->_p->pushCallback(index);
                        }
                    }
                }
            });

            p.checkedObservers[action] = ValueObserver<bool>::create(
                action->observeChecked(),
                [weak, index](bool value)
            {
                if (auto group = weak.lock())
                {
                    const size_t size = group->_p->actions.size();
                    switch (group->_p->buttonType)
                    {
                    case ButtonType::Toggle:
                        if (group->_p->toggleCallback && Callback::Trigger == group->_p->callback)
                        {
                            group->_p->toggleCallback(index, value);
                        }
                        break;
                    case ButtonType::Radio:
                        if (value)
                        {
                            for (size_t i = 0; i < size; ++i)
                            {
                                auto action = group->_p->actions[i];
                                action->setChecked(i == index);
                            }
                            if (group->_p->radioCallback && Callback::Trigger == group->_p->callback)
                            {
                                group->_p->radioCallback(index);
                            }
                        }
                        break;
                    case ButtonType::Exclusive:
                        if (value)
                        {
                            for (size_t i = 0; i < size; ++i)
                            {
                                auto action = group->_p->actions[i];
                                action->setChecked(i == index);
                            }
                            if (group->_p->exclusiveCallback && Callback::Trigger == group->_p->callback)
                            {
                                group->_p->exclusiveCallback(index);
                            }
                        }
                        else
                        {
                            size_t i = 0;
                            for (; i < size; ++i)
                            {
                                if (group->_p->actions[i]->observeChecked()->get())
                                {
                                    break;
                                }
                            }
                            if (size == i &&
                                group->_p->exclusiveCallback &&
                                Callback::Trigger == group->_p->callback)
                            {
                                group->_p->exclusiveCallback(-1);
                            }
                        }
                        break;
                    default: break;
                    }
                }
            });

            p.actions.push_back(action);
        }

        void ActionGroup::removeAction(const std::shared_ptr<Action> & action)
        {
            DJV_PRIVATE_PTR();
            const auto i = std::find(p.actions.begin(), p.actions.end(), action);
            if (i != p.actions.end())
            {
                const auto j = p.clickedObservers.find(*i);
                if (j != p.clickedObservers.end())
                {
                    p.clickedObservers.erase(j);
                }
                const auto k = p.checkedObservers.find(*i);
                if (k != p.checkedObservers.end())
                {
                    p.checkedObservers.erase(k);
                }
                p.actions.erase(i);
            }
        }

        void ActionGroup::clearActions()
        {
            DJV_PRIVATE_PTR();
            p.actions.clear();
            p.clickedObservers.clear();
            p.checkedObservers.clear();
        }

        ButtonType ActionGroup::getButtonType() const
        {
            return _p->buttonType;
        }

        void ActionGroup::setButtonType(ButtonType value)
        {
            DJV_PRIVATE_PTR();
            p.buttonType = value;
            for (size_t i = 0; i < p.actions.size(); ++i)
            {
                p.actions[i]->setButtonType(value);
            }
        }

        void ActionGroup::setChecked(int index, bool value, Callback callback)
        {
            DJV_PRIVATE_PTR();
            p.callback = callback;
            switch (p.buttonType)
            {
            case ButtonType::Push:
                if (index >= 0 && index < static_cast<int>(p.actions.size()))
                {
                    auto action = p.actions[index];
                    action->setChecked(value);
                }
                break;
            case ButtonType::Radio:
                if (value)
                {
                    for (size_t i = 0; i < p.actions.size(); ++i)
                    {
                        // Copy the shared pointer in case the callback clears
                        // the list of actions.
                        auto action = p.actions[i];
                        action->setChecked(i == index);
                    }
                }
                break;
            case ButtonType::Exclusive:
                for (size_t i = 0; i < p.actions.size(); ++i)
                {
                    auto action = p.actions[i];
                    action->setChecked(i == index ? value : false);
                }
                break;
            default: break;
            }
            p.callback = Callback::Trigger;
        }

        void ActionGroup::setPushCallback(const std::function<void(int)> & callback)
        {
            _p->pushCallback = callback;
        }

        void ActionGroup::setToggleCallback(const std::function<void(int, bool)> & callback)
        {
            _p->toggleCallback = callback;
        }

        void ActionGroup::setRadioCallback(const std::function<void(int)> & callback)
        {
            _p->radioCallback = callback;
        }

        void ActionGroup::setExclusiveCallback(const std::function<void(int)> & callback)
        {
            _p->exclusiveCallback = callback;
        }

    } // namespace UI
} // namespace djv
