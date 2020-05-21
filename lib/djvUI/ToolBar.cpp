// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ToolBar.h>

#include <djvUI/Action.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ToolBar::Private
        {
            std::map<std::shared_ptr<Action>, std::shared_ptr<ToolButton> > actionsToButtons;
            struct Observers
            {
                std::shared_ptr<ValueObserver<ButtonType> > buttonType;
                std::shared_ptr<ValueObserver<bool> > checked;
                std::shared_ptr<ValueObserver<std::string> > icon;
                std::shared_ptr<ValueObserver<std::string> > text;
                std::shared_ptr<ValueObserver<bool> > enabled;
                std::shared_ptr<ValueObserver<std::string> > tooltip;
                std::shared_ptr<ValueObserver<bool> > autoRepeat;
            };
            std::map<std::shared_ptr<Action>, Observers> observers;
            std::shared_ptr<HorizontalLayout> layout;
        };

        void ToolBar::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ToolBar");
            setBackgroundRole(ColorRole::BackgroundToolBar);

            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(Layout::Spacing(MetricsRole::None));
            Widget::addChild(p.layout);
        }

        ToolBar::ToolBar() :
            _p(new Private)
        {}

        ToolBar::~ToolBar()
        {}

        std::shared_ptr<ToolBar> ToolBar::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ToolBar>(new ToolBar);
            out->_init(context);
            return out;
        }

        void ToolBar::setStretch(const std::shared_ptr<Widget> & widget, RowStretch value)
        {
            _p->layout->setStretch(widget, value);
        }

        void ToolBar::addSeparator()
        {
            _p->layout->addSeparator();
        }

        void ToolBar::addSpacer()
        {
            _p->layout->addSpacer();
        }

        void ToolBar::addExpander()
        {
            _p->layout->addExpander();
        }

        float ToolBar::getHeightForWidth(float value) const
        {
            const auto& style = _getStyle();
            const glm::vec2 m = getMargin().getSize(style);
            float out = _p->layout->getHeightForWidth(value - m.x) + m.y;
            return out;
        }

        void ToolBar::addAction(const std::shared_ptr<Action> & action)
        {
            Widget::addAction(action);
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto button = ToolButton::create(context);
                p.layout->addChild(button);
                button->setClickedCallback(
                    [action]
                    {
                        action->doClicked();
                    });
                button->setCheckedCallback(
                    [action](bool value)
                    {
                        action->setChecked(value);
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
                    [button](const std::string& value)
                    {
                        button->setIcon(value);
                    });
                /*p.observers[action].text = ValueObserver<std::string>::create(
                    action->observeText(),
                    [button](const std::string & value)
                {
                    button->setText(value);
                });*/
                p.observers[action].enabled = ValueObserver<bool>::create(
                    action->observeEnabled(),
                    [button](bool value)
                    {
                        button->setEnabled(value);
                    });
                p.observers[action].tooltip = ValueObserver<std::string>::create(
                    action->observeTooltip(),
                    [button](const std::string& value)
                    {
                        button->setTooltip(value);
                    });
                p.observers[action].autoRepeat = ValueObserver<bool>::create(
                    action->observeAutoRepeat(),
                    [button](bool value)
                    {
                        button->setAutoRepeat(value);
                    });
            }
        }

        void ToolBar::removeAction(const std::shared_ptr<Action> & action)
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

        void ToolBar::clearActions()
        {
            Widget::clearActions();
            DJV_PRIVATE_PTR();
            for (auto i : p.actionsToButtons)
            {
                p.layout->removeChild(i.second);
            }
            p.actionsToButtons.clear();
            p.observers.clear();
        }

        void ToolBar::addChild(const std::shared_ptr<IObject> & value)
        {
            _p->layout->addChild(value);
        }

        void ToolBar::removeChild(const std::shared_ptr<IObject>& value)
        {
            _p->layout->removeChild(value);
        }

        void ToolBar::clearChildren()
        {
            _p->layout->clearChildren();
        }

        void ToolBar::_preLayoutEvent(Event::PreLayout & event)
        {
            const auto& style = _getStyle();
            const glm::vec2 m = getMargin().getSize(style);
            _setMinimumSize(_p->layout->getMinimumSize() + m);
        }

        void ToolBar::_layoutEvent(Event::Layout & event)
        {
            const BBox2f & g = getGeometry();
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(g, style));
        }

    } // namespace UI
} // namespace djv
