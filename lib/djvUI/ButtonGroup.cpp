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

                void clickedCallback(const std::shared_ptr<IButton>&);
                void checkedCallback(const std::shared_ptr<IButton>&, bool);
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

            void Group::setButtons(const std::vector<std::shared_ptr<IButton> >& buttons, int checked)
            {
                DJV_PRIVATE_PTR();
                p.buttons = buttons;
                for (const auto& i : p.buttons)
                {
                    i->setButtonType(p.buttonType);
                }
                const int size = static_cast<int>(p.buttons.size());
                switch (p.buttonType)
                {
                case ButtonType::Toggle:
                    if (checked >= 0 && checked < size)
                    {
                        p.buttons[checked]->setChecked(true);
                    }
                    break;
                case ButtonType::Radio:
                    if (p.buttons.size())
                    {
                        checked = std::max(0, checked);
                        for (int i = 0; i < size; ++i)
                        {
                            p.buttons[i]->setChecked(i == checked);
                        }
                    }
                    break;
                case ButtonType::Exclusive:
                    if (p.buttons.size())
                    {
                        for (int i = 0; i < size; ++i)
                        {
                            p.buttons[i]->setChecked(i == checked);
                        }
                    }
                    break;
                }
                auto weak = std::weak_ptr<Group>(std::dynamic_pointer_cast<Group>(shared_from_this()));
                for (const auto& i : p.buttons)
                {
                    auto button = i;
                    i->setClickedCallback(
                        [weak, button]
                        {
                            if (auto group = weak.lock())
                            {
                                group->_p->clickedCallback(button);
                            }
                        });
                    i->setCheckedCallback(
                        [weak, button](bool value)
                        {
                            if (auto group = weak.lock())
                            {
                                group->_p->checkedCallback(button, value);
                            }
                        });
                }
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
                        if (_p->radioCallback)
                        {
                            _p->radioCallback(0);
                        }
                    }
                }

                auto weak = std::weak_ptr<Group>(std::dynamic_pointer_cast<Group>(shared_from_this()));
                button->setClickedCallback(
                    [weak, button]
                {
                    if (auto group = weak.lock())
                    {
                        group->_p->clickedCallback(button);
                    }
                });
                button->setCheckedCallback(
                    [weak, button](bool value)
                {
                    if (auto group = weak.lock())
                    {
                        group->_p->checkedCallback(button, value);
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
                    if (ButtonType::Radio == p.buttonType)
                    {
                        if (p.buttons.size() > 0)
                        {
                            if (checked)
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
                        else
                        {
                            if (p.radioCallback)
                            {
                                p.radioCallback(-1);
                            }
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
                int checked = -1;
                for (size_t i = 0; i < p.buttons.size(); ++i)
                {
                    if (-1 == checked && p.buttons[i]->isChecked())
                    {
                        checked = static_cast<int>(i);
                    }
                    p.buttons[i]->setChecked(false);
                    p.buttons[i]->setButtonType(value);
                }
                if (p.buttons.size())
                {
                    switch (p.buttonType)
                    {
                    case ButtonType::Toggle:
                    case ButtonType::Exclusive:
                        if (checked != -1)
                        {
                            p.buttons[checked]->setChecked(true);
                        }
                        break;
                    case ButtonType::Radio:
                        p.buttons[checked != -1 ? checked : 0]->setChecked(true);
                        break;
                    default: break;
                    }
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

            void Group::setChecked(int index, bool value)
            {
                DJV_PRIVATE_PTR();
                switch (p.buttonType)
                {
                case ButtonType::Toggle:
                {
                    if (index >= 0 && index < static_cast<int>(p.buttons.size()))
                    {
                        auto button = p.buttons[index];
                        button->setChecked(value);
                        if (p.toggleCallback)
                        {
                            p.toggleCallback(index, value);
                        }
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
                        if (p.radioCallback)
                        {
                            p.radioCallback(index);
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
                    if (p.exclusiveCallback)
                    {
                        p.exclusiveCallback(value ? index : -1);
                    }
                    break;
                default: break;
                }
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

            void Group::Private::clickedCallback(const std::shared_ptr<IButton>& button)
            {
                const auto i = std::find(buttons.begin(), buttons.end(), button);
                if (i != buttons.end())
                {
                    if (pushCallback)
                    {
                        const int index = static_cast<int>(i - buttons.begin());
                        pushCallback(index);
                    }
                }
            }

            void Group::Private::checkedCallback(const std::shared_ptr<IButton>& button, bool value)
            {
                const auto i = std::find(buttons.begin(), buttons.end(), button);
                if (i != buttons.end())
                {
                    const size_t size = buttons.size();
                    const int index = static_cast<int>(i - buttons.begin());
                    switch (buttonType)
                    {
                    case ButtonType::Toggle:
                        if (toggleCallback)
                        {
                            toggleCallback(index, value);
                        }
                        break;
                    case ButtonType::Radio:
                        for (size_t i = 0; i < size; ++i)
                        {
                            auto button = buttons[i];
                            button->setChecked(i == index);
                        }
                        if (value && radioCallback)
                        {
                            radioCallback(index);
                        }
                        break;
                    case ButtonType::Exclusive:
                        if (value)
                        {
                            for (size_t i = 0; i < size; ++i)
                            {
                                auto button = buttons[i];
                                button->setChecked(i == index);
                            }
                            if (exclusiveCallback)
                            {
                                exclusiveCallback(index);
                            }
                        }
                        else
                        {
                            size_t i = 0;
                            for (; i < size; ++i)
                            {
                                if (buttons[i]->isChecked())
                                {
                                    break;
                                }
                            }
                            if (size == i && exclusiveCallback)
                            {
                                exclusiveCallback(-1);
                            }
                        }
                        break;
                    default: break;
                    }
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
