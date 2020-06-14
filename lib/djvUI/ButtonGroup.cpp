// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ButtonGroup.h>

#include <djvUI/IButton.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            struct Group::Private
            {
                std::vector<std::shared_ptr<IButton> > buttons;
                ButtonType buttonType = ButtonType::Push;
                std::function<void(int)> pushCallback;
                std::function<void(int, bool)> toggleCallback;
                std::function<void(int)> radioCallback;
                std::function<void(int)> exclusiveCallback;
                Callback callback = Callback::Trigger;
            };

            Group::Group() :
                _p(new Private)
            {}

            Group::~Group()
            {
                clearButtons();
            }

            std::shared_ptr<Group> Group::create(ButtonType buttonType)
            {
                auto out = std::shared_ptr<Group>(new Group);
                out->setButtonType(buttonType);
                return out;
            }

            const std::vector<std::shared_ptr<IButton> >& Group::getButtons() const
            {
                return _p->buttons;
            }

            size_t Group::getButtonCount() const
            {
                return _p->buttons.size();
            }

            int Group::getButtonIndex(const std::shared_ptr<IButton>& value) const
            {
                DJV_PRIVATE_PTR();
                const auto i = std::find(p.buttons.begin(), p.buttons.end(), value);
                if (i != p.buttons.end())
                {
                    return static_cast<int>(i - p.buttons.begin());
                }
                return -1;
            }

            void Group::addButton(const std::shared_ptr<IButton>& button)
            {
                DJV_PRIVATE_PTR();
                button->setButtonType(p.buttonType);

                if (ButtonType::Radio == p.buttonType)
                {
                    size_t i = 0;
                    for (; i < p.buttons.size(); ++i)
                    {
                        if (p.buttons[i]->isChecked())
                        {
                            break;
                        }
                    }
                    if (i == p.buttons.size())
                    {
                        button->setChecked(true);
                    }
                }

                auto weak = std::weak_ptr<Group>(std::dynamic_pointer_cast<Group>(shared_from_this()));
                button->setClickedCallback(
                    [weak, button]
                {
                    if (auto group = weak.lock())
                    {
                        const auto i = std::find(group->_p->buttons.begin(), group->_p->buttons.end(), button);
                        if (i != group->_p->buttons.end())
                        {
                            if (group->_p->pushCallback)
                            {
                                const int index = static_cast<int>(i - group->_p->buttons.begin());
                                group->_p->pushCallback(index);
                            }
                        }
                    }
                });

                button->setCheckedCallback(
                    [weak, button](bool value)
                {
                    if (auto group = weak.lock())
                    {
                        const auto i = std::find(group->_p->buttons.begin(), group->_p->buttons.end(), button);
                        if (i != group->_p->buttons.end())
                        {
                            const size_t size = group->_p->buttons.size();
                            const int index = static_cast<int>(i - group->_p->buttons.begin());
                            switch (group->_p->buttonType)
                            {
                            case ButtonType::Toggle:
                                if (group->_p->toggleCallback && Callback::Trigger == group->_p->callback)
                                {
                                    group->_p->toggleCallback(index, value);
                                }
                                break;
                            case ButtonType::Radio:
                                for (size_t i = 0; i < size; ++i)
                                {
                                    auto button = group->_p->buttons[i];
                                    button->setChecked(i == index);
                                }
                                if (value && group->_p->radioCallback && Callback::Trigger == group->_p->callback)
                                {
                                    group->_p->radioCallback(index);
                                }
                                break;
                            case ButtonType::Exclusive:
                                if (value)
                                {
                                    for (size_t i = 0; i < size; ++i)
                                    {
                                        auto button = group->_p->buttons[i];
                                        button->setChecked(i == index);
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
                                        if (group->_p->buttons[i]->isChecked())
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
                    }
                });

                p.buttons.push_back(button);
            }

            void Group::removeButton(const std::shared_ptr<IButton>& button)
            {
                DJV_PRIVATE_PTR();
                const auto i = std::find(p.buttons.begin(), p.buttons.end(), button);
                if (i != p.buttons.end())
                {
                    int index = static_cast<int>(i - p.buttons.begin());
                    const bool checked = (*i)->isChecked();
                    (*i)->setClickedCallback(nullptr);
                    (*i)->setCheckedCallback(nullptr);
                    p.buttons.erase(i);
                    if (ButtonType::Radio == p.buttonType && checked && p.buttons.size() > 0)
                    {
                        if (index >= p.buttons.size())
                        {
                            --index;
                        }
                        auto button = p.buttons[index];
                        button->setChecked(true);
                        if (p.radioCallback)
                        {
                            p.radioCallback(index);
                        }
                    }
                }
            }

            void Group::clearButtons()
            {
                DJV_PRIVATE_PTR();
                auto buttons = p.buttons;
                for (const auto& button : buttons)
                {
                    button->setClickedCallback(nullptr);
                    button->setCheckedCallback(nullptr);
                }
                p.buttons.clear();
            }

            ButtonType Group::getButtonType() const
            {
                return _p->buttonType;
            }

            void Group::setButtonType(ButtonType value)
            {
                DJV_PRIVATE_PTR();
                p.buttonType = value;
                for (size_t i = 0; i < p.buttons.size(); ++i)
                {
                    p.buttons[i]->setButtonType(value);
                }
                if (ButtonType::Radio == p.buttonType)
                {
                    setChecked(0);
                }
            }

            int Group::getChecked() const
            {
                DJV_PRIVATE_PTR();
                for (size_t i = 0; i < p.buttons.size(); ++i)
                {
                    if (p.buttons[i]->isChecked())
                    {
                        return static_cast<int>(i);
                    }
                }
                return -1;
            }

            void Group::setChecked(int index, bool value, Callback callback)
            {
                DJV_PRIVATE_PTR();
                p.callback = callback;
                switch (p.buttonType)
                {
                case ButtonType::Toggle:
                {
                    if (index >= 0 && index < static_cast<int>(p.buttons.size()))
                    {
                        auto button = p.buttons[index];
                        button->setChecked(value);
                    }
                    break;
                }
                case ButtonType::Radio:
                    if (value)
                    {
                        for (size_t i = 0; i < p.buttons.size(); ++i)
                        {
                            auto button = p.buttons[i];
                            button->setChecked(i == index);
                        }
                    }
                    break;
                case ButtonType::Exclusive:
                    if (value)
                    {
                        for (size_t i = 0; i < p.buttons.size(); ++i)
                        {
                            auto button = p.buttons[i];
                            button->setChecked(i == index);
                        }
                    }
                    else if (index >= 0 && index < static_cast<int>(p.buttons.size()))
                    {
                        auto button = p.buttons[index];
                        button->setChecked(false);
                    }
                    break;
                default: break;
                }
                p.callback = Callback::Trigger;
            }

            void Group::setPushCallback(const std::function<void(int)>& callback)
            {
                _p->pushCallback = callback;
            }

            void Group::setToggleCallback(const std::function<void(int, bool)>& callback)
            {
                _p->toggleCallback = callback;
            }

            void Group::setRadioCallback(const std::function<void(int)>& callback)
            {
                _p->radioCallback = callback;
            }

            void Group::setExclusiveCallback(const std::function<void(int)>& callback)
            {
                _p->exclusiveCallback = callback;
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
