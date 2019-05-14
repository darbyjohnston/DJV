//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvUI/ActionButton.h>

#include <djvUI/Action.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>

#include <djvAV/Render2D.h>

#include <djvCore/String.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            struct ActionButton::Private
            {
                std::shared_ptr<Action> action;
                std::shared_ptr<Icon> icon;
                std::shared_ptr<Label> textLabel;
                std::shared_ptr<Label> shortcutsLabel;
                std::shared_ptr<HorizontalLayout> layout;
                std::shared_ptr<ValueObserver<ButtonType> > buttonTypeObserver;
                std::shared_ptr<ValueObserver<bool> > checkedObserver;
                std::shared_ptr<ValueObserver<std::string> > iconObserver;
                std::shared_ptr<ValueObserver<std::string> > textObserver;
                std::shared_ptr<ListObserver<std::shared_ptr<Shortcut> > > shortcutsObserver;
                std::shared_ptr<ValueObserver<bool> > enabledObserver;
                std::shared_ptr<ValueObserver<std::string> > tooltipObserver;
            };

            void ActionButton::_init(Context * context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::ActionButton");

                p.icon = Icon::create(context);
                p.textLabel = Label::create(context);
                p.textLabel->setTextHAlign(TextHAlign::Left);
                p.shortcutsLabel = Label::create(context);

                p.layout = HorizontalLayout::create(context);
                p.layout->setMargin(MetricsRole::MarginSmall);
                p.layout->addChild(p.icon);
                p.layout->addChild(p.textLabel);
                p.layout->setStretch(p.textLabel, RowStretch::Expand);
                p.layout->addChild(p.shortcutsLabel);
                addChild(p.layout);

                _actionUpdate();

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

            std::shared_ptr<ActionButton> ActionButton::create(Context * context)
            {
                auto out = std::shared_ptr<ActionButton>(new ActionButton);
                out->_init(context);
                return out;
            }

            std::shared_ptr<ActionButton> ActionButton::create(const std::shared_ptr<Action> & action, Context * context)
            {
                auto out = std::shared_ptr<ActionButton>(new ActionButton);
                out->_init(context);
                out->addAction(action);
                return out;
            }

            bool ActionButton::hasShowText() const
            {
                return _p->textLabel->isVisible();
            }

            void ActionButton::setShowText(bool value)
            {
                _p->textLabel->setVisible(value);
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
                DJV_PRIVATE_PTR();
                p.icon->setIconColorRole(value ? ColorRole::Checked : getForegroundColorRole());
                p.textLabel->setTextColorRole(value ? ColorRole::Checked : getForegroundColorRole());
                p.shortcutsLabel->setTextColorRole(value ? ColorRole::Checked : getForegroundColorRole());
            }

            void ActionButton::addAction(const std::shared_ptr<Action> & value)
            {
                IButton::addAction(value);
                _actionUpdate();
            }

            void ActionButton::removeAction(const std::shared_ptr<Action> & value)
            {
                IButton::removeAction(value);
                _actionUpdate();
            }

            void ActionButton::clearActions()
            {
                IButton::clearActions();
                _actionUpdate();
            }

            void ActionButton::_preLayoutEvent(Event::PreLayout & event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void ActionButton::_layoutEvent(Event::Layout &)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void ActionButton::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const BBox2f& g = getGeometry();
                auto render = _getRender();
                auto style = _getStyle();
                if (_isPressed())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Pressed)));
                    render->drawRect(g);
                }
                else if (_isHovered())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Hovered)));
                    render->drawRect(g);
                }
            }

            void ActionButton::_actionUpdate()
            {
                DJV_PRIVATE_PTR();
                const auto & actions = getActions();
                if (actions.size())
                {
                    p.action = actions.front();
                    auto weak = std::weak_ptr<ActionButton>(std::dynamic_pointer_cast<ActionButton>(shared_from_this()));
                    p.buttonTypeObserver = ValueObserver<ButtonType>::create(
                        p.action->observeButtonType(),
                        [weak](ButtonType value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->setButtonType(value);
                        }
                    });
                    p.checkedObserver = ValueObserver<bool>::create(
                        p.action->observeChecked(),
                        [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->setChecked(value);
                        }
                    });
                    p.iconObserver = ValueObserver<std::string>::create(
                        p.action->observeIcon(),
                        [weak](const std::string & value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->icon->setIcon(value);
                        }
                    });
                    p.textObserver = ValueObserver<std::string>::create(
                        p.action->observeText(),
                        [weak](const std::string & value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->textLabel->setText(value);
                        }
                    });
                    p.shortcutsObserver = ListObserver<std::shared_ptr<Shortcut> >::create(
                        p.action->observeShortcuts(),
                        [weak](const std::vector<std::shared_ptr<Shortcut> > & value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto textSystem = widget->getContext()->getSystemT<TextSystem>())
                            {
                                std::vector<std::string> labels;
                                for (const auto & i : value)
                                {
                                    labels.push_back(Shortcut::getText(
                                        i->observeShortcutKey()->get(),
                                        i->observeShortcutModifiers()->get(),
                                        textSystem));
                                }
                                widget->_p->shortcutsLabel->setText(String::join(labels, ", "));
                            }
                        }
                    });
                    p.enabledObserver = ValueObserver<bool>::create(
                        p.action->observeEnabled(),
                        [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->setEnabled(value);
                        }
                    });
                    p.tooltipObserver = ValueObserver<std::string>::create(
                        p.action->observeTooltip(),
                        [weak](const std::string & value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->setTooltip(value);
                        }
                    });
                }
                else
                {
                    p.action.reset();
                    p.buttonTypeObserver.reset();
                    p.checkedObserver.reset();
                    p.iconObserver.reset();
                    p.textObserver.reset();
                    p.shortcutsObserver.reset();
                    p.enabledObserver.reset();
                    p.tooltipObserver.reset();
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace djv

