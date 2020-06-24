// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ActionButton.h>

#include <djvUI/Action.h>
#include <djvUI/DrawUtil.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>

#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/String.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            namespace
            {
                class CheckBox : public Widget
                {
                    DJV_NON_COPYABLE(CheckBox);

                protected:
                    void _init(const std::shared_ptr<Context>&);
                    CheckBox();

                public:
                    virtual ~CheckBox();

                    static std::shared_ptr<CheckBox> create(const std::shared_ptr<Context>&);

                    void setChecked(bool);

                protected:
                    void _preLayoutEvent(Event::PreLayout&) override;
                    void _paintEvent(Event::Paint&) override;

                private:
                    bool _checked = false;
                };

                void CheckBox::_init(const std::shared_ptr<Context>& context)
                {
                    Widget::_init(context);
                }

                CheckBox::CheckBox()
                {}

                CheckBox::~CheckBox()
                {}

                std::shared_ptr<CheckBox> CheckBox::create(const std::shared_ptr<Context>& context)
                {
                    auto out = std::shared_ptr<CheckBox>(new CheckBox);
                    out->_init(context);
                    return out;
                }

                void CheckBox::setChecked(bool value)
                {
                    if (value == _checked)
                        return;
                    _checked = value;
                    _redraw();
                }

                void CheckBox::_preLayoutEvent(Event::PreLayout&)
                {
                    const auto& style = _getStyle();
                    const float m = style->getMetric(MetricsRole::MarginInside);
                    const float b = style->getMetric(MetricsRole::Border);
                    const float is = style->getMetric(MetricsRole::IconSmall);
                    _setMinimumSize(glm::vec2(is + m * 2.F + b * 4.F, is + m * 2.F + b * 4.F));
                }

                void CheckBox::_paintEvent(Event::Paint&)
                {
                    const auto& style = _getStyle();
                    const float m = style->getMetric(MetricsRole::MarginInside);
                    const float b = style->getMetric(MetricsRole::Border);
                    const float is = style->getMetric(MetricsRole::IconSmall);
                    const BBox2f g = getGeometry().margin(-b);
                    const float size = is + m * 2.F;
                    const BBox2f g2 = BBox2f(g.min.x, floorf(g.min.y + g.h() / 2.F - size / 2.F), size, size).margin(-m);
                    const auto& render = _getRender();
                    render->setFillColor(style->getColor(ColorRole::Border));
                    drawBorder(render, g2, b);
                    render->setFillColor(style->getColor(_checked ? ColorRole::Checked : ColorRole::Trough));
                    render->drawRect(g2.margin(-b));
                }

            } // namespace

            struct ActionButton::Private
            {
                std::shared_ptr<Action> action;
                std::shared_ptr<CheckBox> checkBox;
                std::shared_ptr<Icon> icon;
                std::shared_ptr<Label> textLabel;
                std::shared_ptr<Label> shortcutsLabel;
                std::shared_ptr<HorizontalLayout> layout;
                std::shared_ptr<ValueObserver<std::string> > iconObserver;
                std::shared_ptr<ValueObserver<std::string> > textObserver;
                std::shared_ptr<ListObserver<std::shared_ptr<Shortcut> > > shortcutsObserver;
            };

            void ActionButton::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::Button::ActionButton");

                p.checkBox = CheckBox::create(context);
                p.icon = Icon::create(context);
                p.textLabel = Label::create(context);
                p.textLabel->setTextHAlign(TextHAlign::Left);
                p.textLabel->setMargin(MetricsRole::MarginSmall);
                p.shortcutsLabel = Label::create(context);
                p.shortcutsLabel->setMargin(MetricsRole::MarginSmall);

                p.layout = HorizontalLayout::create(context);
                p.layout->setMargin(MetricsRole::MarginInside);
                p.layout->setSpacing(MetricsRole::None);
                p.layout->addChild(p.checkBox);
                p.layout->addChild(p.icon);
                p.layout->addChild(p.textLabel);
                p.layout->setStretch(p.textLabel, RowStretch::Expand);
                p.layout->addChild(p.shortcutsLabel);
                addChild(p.layout);

                _actionUpdate();
                _widgetUpdate();

                auto weak = std::weak_ptr<ActionButton>(std::dynamic_pointer_cast<ActionButton>(shared_from_this()));
                setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->action)
                            {
                                widget->_p->action->doClicked();
                            }
                        }
                    });
                setCheckedCallback(
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->action)
                            {
                                widget->_p->action->setChecked(value);
                            }
                        }
                    });
            }

            ActionButton::ActionButton() :
                _p(new Private)
            {}

            ActionButton::~ActionButton()
            {}

            std::shared_ptr<ActionButton> ActionButton::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ActionButton>(new ActionButton);
                out->_init(context);
                return out;
            }

            bool ActionButton::hasShowShortcuts() const
            {
                return _p->shortcutsLabel->isVisible();
            }

            void ActionButton::setShowShortcuts(bool value)
            {
                _p->shortcutsLabel->setVisible(value);
            }

            void ActionButton::setChecked(bool value)
            {
                IButton::setChecked(value);
                _p->checkBox->setChecked(value);
            }

            void ActionButton::setButtonType(ButtonType value)
            {
                IButton::setButtonType(value);
                _widgetUpdate();
            }

            void ActionButton::addAction(const std::shared_ptr<Action>& value)
            {
                IButton::addAction(value);
                _actionUpdate();
                _widgetUpdate();
            }

            void ActionButton::removeAction(const std::shared_ptr<Action>& value)
            {
                IButton::removeAction(value);
                _actionUpdate();
                _widgetUpdate();
            }

            void ActionButton::clearActions()
            {
                IButton::clearActions();
                _actionUpdate();
                _widgetUpdate();
            }

            void ActionButton::_preLayoutEvent(Event::PreLayout& event)
            {
                DJV_PRIVATE_PTR();
                _setMinimumSize(p.layout->getMinimumSize());
            }

            void ActionButton::_layoutEvent(Event::Layout&)
            {
                DJV_PRIVATE_PTR();
                p.layout->setGeometry(getGeometry());
            }

            void ActionButton::_paintEvent(Event::Paint& event)
            {
                IButton::_paintEvent(event);
                const auto& style = _getStyle();
                const BBox2f& g = getGeometry();
                const auto& render = _getRender();
                if (_isPressed())
                {
                    render->setFillColor(style->getColor(ColorRole::Pressed));
                    render->drawRect(g);
                }
                else if (_isHovered())
                {
                    render->setFillColor(style->getColor(ColorRole::Hovered));
                    render->drawRect(g);
                }
            }

            void ActionButton::_actionUpdate()
            {
                DJV_PRIVATE_PTR();
                const auto& actions = getActions();
                if (actions.size())
                {
                    p.action = actions.front();
                    auto weak = std::weak_ptr<ActionButton>(std::dynamic_pointer_cast<ActionButton>(shared_from_this()));
                    p.iconObserver = ValueObserver<std::string>::create(
                        p.action->observeIcon(),
                        [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->icon->setIcon(value);
                        }
                    });
                    p.textObserver = ValueObserver<std::string>::create(
                        p.action->observeText(),
                        [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->textLabel->setText(value);
                        }
                    });
                    p.shortcutsObserver = ListObserver<std::shared_ptr<Shortcut> >::create(
                        p.action->observeShortcuts(),
                        [weak](const std::vector<std::shared_ptr<Shortcut> >& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto context = widget->getContext().lock())
                            {
                                auto textSystem = context->getSystemT<TextSystem>();
                                std::vector<std::string> labels;
                                for (const auto& i : value)
                                {
                                    const auto& shortcut = i->observeShortcut()->get();
                                    labels.push_back(Shortcut::getText(shortcut.key, shortcut.modifiers, textSystem));
                                }
                                widget->_p->shortcutsLabel->setText(String::join(labels, ", "));
                            }
                        }
                    });
                }
                else
                {
                    p.action.reset();
                    p.iconObserver.reset();
                    p.textObserver.reset();
                    p.shortcutsObserver.reset();
                }
            }

            void ActionButton::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                if (p.action)
                {
                    _p->checkBox->setVisible(p.action->observeButtonType()->get() != ButtonType::Push);
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace djv

