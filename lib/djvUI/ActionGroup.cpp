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

            void clickedCallback(const std::shared_ptr<Action>&);
            void checkedCallback(const std::shared_ptr<Action>&, bool);
        };

        ActionGroup::ActionGroup(ButtonType buttonType) :
            _p(new Private)
        {
            _p->buttonType = buttonType;
        }

        ActionGroup::~ActionGroup()
        {
            DJV_PRIVATE_PTR();
            for (const auto& action : p.actions)
            {
                action->setClickedCallback(nullptr);
                action->setCheckedCallback(nullptr);
            }
        }

        std::shared_ptr<ActionGroup> ActionGroup::create(ButtonType buttonType)
        {
            auto out = std::shared_ptr<ActionGroup>(new ActionGroup(buttonType));
            return out;
        }

        ButtonType ActionGroup::getButtonType() const
        {
            return _p->buttonType;
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
            DJV_PRIVATE_PTR();
            const auto i = std::find(p.actions.begin(), p.actions.end(), value);
            if (i != p.actions.end())
            {
                return static_cast<int>(i - p.actions.begin());
            }
            return -1;
        }

        void ActionGroup::setActions(const std::vector<std::shared_ptr<Action> >& actions)
        {
            DJV_PRIVATE_PTR();

            for (const auto& action : p.actions)
            {
                action->setClickedCallback(nullptr);
                action->setCheckedCallback(nullptr);
            }

            p.actions = actions;

            auto weak = std::weak_ptr<ActionGroup>(std::dynamic_pointer_cast<ActionGroup>(shared_from_this()));
            for (size_t i = 0; i < p.actions.size(); ++i)
            {
                const auto& action = p.actions[i];
                action->setButtonType(p.buttonType);

                switch (p.buttonType)
                {
                case ButtonType::Push:
                case ButtonType::Toggle:
                case ButtonType::Exclusive:
                    action->setChecked(false);
                    break;
                case ButtonType::Radio:
                    action->setChecked(0 == i);
                    break;
                default: break;
                }

                action->setClickedCallback(
                    [weak, action]
                    {
                        if (auto group = weak.lock())
                        {
                            group->_p->clickedCallback(action);
                        }
                    });
                action->setCheckedCallback(
                    [weak, action](bool value)
                    {
                        if (auto group = weak.lock())
                        {
                            group->_p->checkedCallback(action, value);
                        }
                    });
            }
        }

        void ActionGroup::clearActions()
        {
            DJV_PRIVATE_PTR();
            for (const auto& action : p.actions)
            {
                action->setClickedCallback(nullptr);
                action->setCheckedCallback(nullptr);
            }
            p.actions.clear();
        }

        int ActionGroup::getChecked() const
        {
            DJV_PRIVATE_PTR();
            for (size_t i = 0; i < p.actions.size(); ++i)
            {
                if (p.actions[i]->observeChecked()->get())
                {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        void ActionGroup::setChecked(int index, bool value)
        {
            DJV_PRIVATE_PTR();
            switch (p.buttonType)
            {
            case ButtonType::Toggle:
                if (index >= 0 && index < static_cast<int>(p.actions.size()))
                {
                    p.actions[index]->setChecked(value);
                }
                break;
            case ButtonType::Radio:
                for (size_t i = 0; i < p.actions.size(); ++i)
                {
                    p.actions[i]->setChecked(i == index);
                }
                break;
            case ButtonType::Exclusive:
                for (size_t i = 0; i < p.actions.size(); ++i)
                {
                    p.actions[i]->setChecked(i == index ? value : false);
                }
                break;
            default: break;
            }
        }

        void ActionGroup::setPushCallback(const std::function<void(int)>& callback)
        {
            _p->pushCallback = callback;
        }

        void ActionGroup::setToggleCallback(const std::function<void(int, bool)>& callback)
        {
            _p->toggleCallback = callback;
        }

        void ActionGroup::setRadioCallback(const std::function<void(int)>& callback)
        {
            _p->radioCallback = callback;
        }

        void ActionGroup::setExclusiveCallback(const std::function<void(int)>& callback)
        {
            _p->exclusiveCallback = callback;
        }

        void ActionGroup::Private::clickedCallback(const std::shared_ptr<Action>& action)
        {
            const auto i = std::find(actions.begin(), actions.end(), action);
            if (i != actions.end())
            {
                if (pushCallback)
                {
                    const int index = static_cast<int>(i - actions.begin());
                    pushCallback(index);
                }
            }
        }

        void ActionGroup::Private::checkedCallback(const std::shared_ptr<Action>& action, bool value)
        {
            const auto i = std::find(actions.begin(), actions.end(), action);
            if (i != actions.end())
            {
                const int index = static_cast<int>(i - actions.begin());
                switch (buttonType)
                {
                case ButtonType::Toggle:
                    if (toggleCallback)
                    {
                        toggleCallback(index, value);
                    }
                    break;
                case ButtonType::Radio:
                    for (size_t i = 0; i < actions.size(); ++i)
                    {
                        actions[i]->setChecked(static_cast<int>(i) == index);
                    }
                    if (radioCallback)
                    {
                        radioCallback(index);
                    }
                    break;
                case ButtonType::Exclusive:
                    for (size_t i = 0; i < actions.size(); ++i)
                    {
                        actions[i]->setChecked(value ? static_cast<int>(i) == index : false);
                    }
                    if (exclusiveCallback)
                    {
                        exclusiveCallback(value ? index : -1);
                    }
                default: break;
                }
            }
        }

    } // namespace UI
} // namespace djv
