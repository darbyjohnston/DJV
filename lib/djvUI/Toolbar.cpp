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

#include <djvUI/Toolbar.h>

#include <djvUI/Action.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Toolbar::Private
        {
            std::map<std::shared_ptr<Action>, std::shared_ptr<Button::Tool> > actionsToButtons;
            struct Observers
            {
                std::shared_ptr<ValueObserver<ButtonType> > buttonType;
                std::shared_ptr<ValueObserver<bool> > checked;
                std::shared_ptr<ValueObserver<FileSystem::Path> > icon;
                std::shared_ptr<ValueObserver<bool> > enabled;
                std::shared_ptr<ValueObserver<std::string> > tooltip;
            };
            std::map<std::shared_ptr<Action>, Observers> observers;
            std::shared_ptr<Layout::Horizontal> layout;
        };

        void Toolbar::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::UI::Toolbar");

            _p->layout = Layout::Horizontal::create(context);
            _p->layout->setSpacing(Style::MetricsRole::None);
            _p->layout->setParent(shared_from_this());
        }

        Toolbar::Toolbar() :
            _p(new Private)
        {}

        Toolbar::~Toolbar()
        {}

        std::shared_ptr<Toolbar> Toolbar::create(Context * context)
        {
            auto out = std::shared_ptr<Toolbar>(new Toolbar);
            out->_init(context);
            return out;
        }

        void Toolbar::addWidget(const std::shared_ptr<Widget>& value, Layout::RowStretch stretch)
        {
            _p->layout->addWidget(value, stretch);
        }

        void Toolbar::removeWidget(const std::shared_ptr<Widget>& value)
        {
            _p->layout->removeWidget(value);
        }

        void Toolbar::clearWidgets()
        {
            _p->layout->clearWidgets();
            clearActions();
        }

        float Toolbar::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void Toolbar::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void Toolbar::_layoutEvent(Event::Layout& event)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void Toolbar::addAction(const std::shared_ptr<Action>& action)
        {
            Widget::addAction(action);
            auto button = Button::Tool::create(getContext());
            _p->layout->addWidget(button);
            button->setClickedCallback(
                [action]
            {
                action->doClicked();
            });
            button->setCheckedCallback(
                [action](bool value)
            {
                action->setChecked(value);
                action->doChecked();
            });
            _p->actionsToButtons[action] = button;
            _p->observers[action].buttonType = ValueObserver<ButtonType>::create(
                action->getButtonType(),
                [button](ButtonType value)
            {
                button->setButtonType(value);
            });
            _p->observers[action].checked = ValueObserver<bool>::create(
                action->isChecked(),
                [button](bool value)
            {
                button->setChecked(value);
            });
            _p->observers[action].icon = ValueObserver<FileSystem::Path>::create(
                action->getIcon(),
                [button](const FileSystem::Path & value)
            {
                button->setIcon(value);
            });
            _p->observers[action].enabled = ValueObserver<bool>::create(
                action->isEnabled(),
                [button](bool value)
            {
                button->setEnabled(value);
            });
            _p->observers[action].tooltip = ValueObserver<std::string>::create(
                action->getTooltip(),
                [button](const std::string & value)
            {
                button->setTooltip(value);
            });
        }

        void Toolbar::removeAction(const std::shared_ptr<Action>& action)
        {
            Widget::removeAction(action);
            const auto i = _p->actionsToButtons.find(action);
            if (i != _p->actionsToButtons.end())
            {
                _p->actionsToButtons.erase(i);
            }
            const auto j = _p->observers.find(action);
            if (j != _p->observers.end())
            {
                _p->observers.erase(j);
            }
        }

        void Toolbar::clearActions()
        {
            Widget::clearActions();
            for (auto i : _p->actionsToButtons)
            {
                _p->layout->removeWidget(i.second);
            }
            _p->actionsToButtons.clear();
            _p->observers.clear();
        }

        void Toolbar::addSeparator()
        {
            _p->layout->addSeparator();
        }

        void Toolbar::addSpacer()
        {
            _p->layout->addSpacer();
        }

        void Toolbar::addExpander()
        {
            _p->layout->addExpander();
        }

    } // namespace UI
} // namespace djv
