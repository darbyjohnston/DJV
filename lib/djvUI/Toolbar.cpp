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
            std::map<std::shared_ptr<Action>, std::shared_ptr<ToolButton> > actionsToButtons;
            struct Observers
            {
                std::shared_ptr<ValueObserver<ButtonType> > buttonType;
                std::shared_ptr<ValueObserver<bool> > checked;
                std::shared_ptr<ValueObserver<std::string> > icon;
                std::shared_ptr<ValueObserver<bool> > enabled;
                std::shared_ptr<ValueObserver<std::string> > tooltip;
            };
            std::map<std::shared_ptr<Action>, Observers> observers;
            std::shared_ptr<HorizontalLayout> layout;
        };

        void Toolbar::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::UI::Toolbar");

            DJV_PRIVATE_PTR();
            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(Style::MetricsRole::None);
            p.layout->setParent(shared_from_this());
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
            float out = 0.f;
            if (auto style = _getStyle().lock())
            {
                const glm::vec2 m = getMargin().getSize(style);
                out = _p->layout->getHeightForWidth(value - m.x) + m.y;
            }
            return out;
        }

        void Toolbar::_preLayoutEvent(Event::PreLayout& event)
        {
            if (auto style = _getStyle().lock())
            {
                const glm::vec2 m = getMargin().getSize(style);
                _setMinimumSize(_p->layout->getMinimumSize() + m);
            }
        }

        void Toolbar::_layoutEvent(Event::Layout& event)
        {
            if (auto style = _getStyle().lock())
            {
                const BBox2f & g = getGeometry();
                _p->layout->setGeometry(getMargin().bbox(g, style));
            }
        }

        void Toolbar::addAction(const std::shared_ptr<Action>& action)
        {
            Widget::addAction(action);
            auto button = ToolButton::create(getContext());
            DJV_PRIVATE_PTR();
            p.layout->addWidget(button);
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
            p.actionsToButtons[action] = button;
            p.observers[action].buttonType = ValueObserver<ButtonType>::create(
                action->observeButtonType(),
                [button](ButtonType value)
            {
                button->setButtonType(value);
            });
            p.observers[action].checked = ValueObserver<bool>::create(
                action->observeChecked(),
                [button](bool value)
            {
                button->setChecked(value);
            });
            p.observers[action].icon = ValueObserver<std::string>::create(
                action->observeIcon(),
                [button](const std::string & value)
            {
                button->setIcon(value);
            });
            p.observers[action].enabled = ValueObserver<bool>::create(
                action->observeEnabled(),
                [button](bool value)
            {
                button->setEnabled(value);
            });
            p.observers[action].tooltip = ValueObserver<std::string>::create(
                action->observeTooltip(),
                [button](const std::string & value)
            {
                button->setTooltip(value);
            });
        }

        void Toolbar::removeAction(const std::shared_ptr<Action>& action)
        {
            Widget::removeAction(action);
            DJV_PRIVATE_PTR();
            const auto i = p.actionsToButtons.find(action);
            if (i != p.actionsToButtons.end())
            {
                p.actionsToButtons.erase(i);
            }
            const auto j = p.observers.find(action);
            if (j != p.observers.end())
            {
                p.observers.erase(j);
            }
        }

        void Toolbar::clearActions()
        {
            Widget::clearActions();
            DJV_PRIVATE_PTR();
            for (auto i : p.actionsToButtons)
            {
                p.layout->removeWidget(i.second);
            }
            p.actionsToButtons.clear();
            p.observers.clear();
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
