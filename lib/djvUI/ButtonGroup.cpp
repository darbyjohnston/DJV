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
                std::function<void(int)> clickedCallback;
                std::function<void(int, bool)> checkedCallback;
                std::function<void(int)> radioCallback;
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
                const auto i = std::find(_p->buttons.begin(), _p->buttons.end(), value);
                if (i != _p->buttons.end())
                {
                    return static_cast<int>(i - _p->buttons.begin());
                }
                return -1;
            }

            void Group::addButton(const std::shared_ptr<IButton>& button)
            {
                button->setButtonType(_p->buttonType);

                if (ButtonType::Radio == _p->buttonType)
                {
                    size_t i = 0;
                    for (; i < _p->buttons.size(); ++i)
                    {
                        if (_p->buttons[i]->isChecked())
                        {
                            break;
                        }
                    }
                    if (i == _p->buttons.size())
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
                            if (group->_p->clickedCallback)
                            {
                                const int index = static_cast<int>(i - group->_p->buttons.begin());
                                group->_p->clickedCallback(index);
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
                            const int index = static_cast<int>(i - group->_p->buttons.begin());
                            if (ButtonType::Radio == group->_p->buttonType)
                            {
                                for (size_t i = 0; i < group->_p->buttons.size(); ++i)
                                {
                                    group->_p->buttons[i]->setChecked(i == index);
                                }

                                if (value && group->_p->radioCallback)
                                {
                                    group->_p->radioCallback(index);
                                }
                            }

                            if (group->_p->checkedCallback)
                            {
                                group->_p->checkedCallback(index, true);
                            }
                        }
                    }
                });

                _p->buttons.push_back(button);
            }

            void Group::removeButton(const std::shared_ptr<IButton>& button)
            {
                const auto i = std::find(_p->buttons.begin(), _p->buttons.end(), button);
                if (i != _p->buttons.end())
                {
                    (*i)->setClickedCallback(nullptr);
                    (*i)->setCheckedCallback(nullptr);
                    _p->buttons.erase(i);
                }
            }

            void Group::clearButtons()
            {
                auto buttons = _p->buttons;
                for (const auto& button : buttons)
                {
                    button->setClickedCallback(nullptr);
                    button->setCheckedCallback(nullptr);
                }
                _p->buttons.clear();
            }

            ButtonType Group::getButtonType() const
            {
                return _p->buttonType;
            }

            void Group::setButtonType(ButtonType value)
            {
                _p->buttonType = value;

                for (size_t i = 0; i < _p->buttons.size(); ++i)
                {
                    _p->buttons[i]->setButtonType(value);
                }

                if (ButtonType::Radio == _p->buttonType)
                {
                    setChecked(0);
                }
            }

            int Group::getChecked() const
            {
                for (size_t i = 0; i < _p->buttons.size(); ++i)
                {
                    if (_p->buttons[i]->isChecked())
                    {
                        return static_cast<int>(i);
                    }
                }
                return -1;
            }

            void Group::setChecked(int index, bool value)
            {
                if (index >= 0 && index < static_cast<int>(_p->buttons.size()))
                {
                    if (_p->buttons[index]->isChecked() != value)
                    {
                        if (ButtonType::Radio == _p->buttonType)
                        {
                            for (size_t i = 0; i < _p->buttons.size(); ++i)
                            {
                                _p->buttons[i]->setChecked(i == index);
                            }

                            if (_p->radioCallback)
                            {
                                _p->radioCallback(index);
                            }
                        }
                        else
                        {
                            _p->buttons[index]->setChecked(value);
                        }

                        if (_p->checkedCallback)
                        {
                            _p->checkedCallback(index, value);
                        }
                    }
                }
            }

            void Group::setClickedCallback(const std::function<void(int)>& callback)
            {
                _p->clickedCallback = callback;
            }

            void Group::setCheckedCallback(const std::function<void(int, bool)>& callback)
            {
                _p->checkedCallback = callback;
            }

            void Group::setRadioCallback(const std::function<void(int)>& callback)
            {
                _p->radioCallback = callback;
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
