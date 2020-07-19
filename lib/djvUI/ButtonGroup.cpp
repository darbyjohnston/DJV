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

            Group::Group(ButtonType buttonType) :
                _p(new Private)
            {
                _p->buttonType = buttonType;
            }

            Group::~Group()
            {
                DJV_PRIVATE_PTR();
                for (const auto& button : p.buttons)
                {
                    button->setClickedCallback(nullptr);
                    button->setCheckedCallback(nullptr);
                }
            }

            std::shared_ptr<Group> Group::create(ButtonType buttonType)
            {
                auto out = std::shared_ptr<Group>(new Group(buttonType));
                return out;
            }

            ButtonType Group::getButtonType() const
            {
                return _p->buttonType;
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

            void Group::setButtons(const std::vector<std::shared_ptr<IButton> >& buttons)
            {
                DJV_PRIVATE_PTR();

                for (const auto& button : p.buttons)
                {
                    button->setClickedCallback(nullptr);
                    button->setCheckedCallback(nullptr);
                }

                p.buttons = buttons;

                auto weak = std::weak_ptr<Group>(std::dynamic_pointer_cast<Group>(shared_from_this()));
                for (size_t i = 0; i < p.buttons.size(); ++i)
                {
                    auto button = p.buttons[i];
                    button->setButtonType(p.buttonType);

                    switch (p.buttonType)
                    {
                    case ButtonType::Push:
                    case ButtonType::Toggle:
                    case ButtonType::Exclusive:
                        button->setChecked(false);
                        break;
                    case ButtonType::Radio:
                        button->setChecked(0 == i);
                        break;
                    default: break;
                    }

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
                }
            }

            void Group::clearButtons()
            {
                DJV_PRIVATE_PTR();
                for (const auto& i : p.buttons)
                {
                    i->setClickedCallback(nullptr);
                    i->setCheckedCallback(nullptr);
                }
                p.buttons.clear();
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
                    if (index >= 0 && index < static_cast<int>(p.buttons.size()))
                    {
                        p.buttons[index]->setChecked(value);
                    }
                    break;
                case ButtonType::Radio:
                    if (value)
                    {
                        for (size_t i = 0; i < p.buttons.size(); ++i)
                        {
                            p.buttons[i]->setChecked(static_cast<int>(i) == index);
                        }
                    }
                    break;
                case ButtonType::Exclusive:
                    for (size_t i = 0; i < p.buttons.size(); ++i)
                    {
                        p.buttons[i]->setChecked(i == index);
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
                        for (size_t i = 0; i < buttons.size(); ++i)
                        {
                            buttons[i]->setChecked(static_cast<int>(i) == index);
                        }
                        if (radioCallback)
                        {
                            radioCallback(index);
                        }
                        break;
                    case ButtonType::Exclusive:
                        for (size_t i = 0; i < buttons.size(); ++i)
                        {
                            buttons[i]->setChecked(value ? static_cast<int>(i) == index : false);
                        }
                        if (exclusiveCallback)
                        {
                            exclusiveCallback(value ? index : -1);
                        }
                    default: break;
                    }
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
