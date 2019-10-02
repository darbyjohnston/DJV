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

#include <djvUI/ToolButton.h>

#include <djvUI/Action.h>
#include <djvUI/Border.h>
#include <djvUI/DrawUtil.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>

#include <djvAV/Render2D.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            struct Tool::Private
            {
                std::string icon;
                std::string checkedIcon;
                std::shared_ptr<Icon> iconWidget;
                std::shared_ptr<Label> label;
                TextHAlign textHAlign = TextHAlign::Left;
                std::string font;
                std::string fontFace;
                MetricsRole fontSizeRole = MetricsRole::FontMedium;
                std::shared_ptr<Action> action;
                bool textFocusEnabled = false;
                std::shared_ptr<HorizontalLayout> layout;
                std::shared_ptr<Border> border;
                std::shared_ptr<ValueObserver<std::string> > iconObserver;
            };

            void Tool::_init(const std::shared_ptr<Context>& context)
            {
                IButton::_init(context);

                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Button::Tool");
                setVAlign(VAlign::Center);

                p.layout = HorizontalLayout::create(context);
                p.layout->setMargin(Layout::Margin(MetricsRole::MarginSmall));
                p.border = Layout::Border::create(context);
                p.border->setBorderColorRole(UI::ColorRole::None);
                p.border->addChild(p.layout);
                addChild(p.border);

                _actionUpdate();
                _iconUpdate();

                auto weak = std::weak_ptr<Tool>(std::dynamic_pointer_cast<Tool>(shared_from_this()));
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

            Tool::Tool() :
                _p(new Private)
            {}

            Tool::~Tool()
            {}

            std::shared_ptr<Tool> Tool::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Tool>(new Tool);
                out->_init(context);
                return out;
            }

            void Tool::setIcon(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.icon)
                    return;
                p.icon = value;
                if (!value.empty())
                {
                    if (auto context = getContext().lock())
                    {
                        if (!p.iconWidget)
                        {
                            p.iconWidget = Icon::create(context);
                            p.iconWidget->setVAlign(VAlign::Center);
                            p.layout->addChild(p.iconWidget);
                            p.iconWidget->moveToFront();
                        }
                        _iconUpdate();
                    }
                }
                else
                {
                    p.layout->removeChild(p.iconWidget);
                    p.iconWidget.reset();
                }
            }

            void Tool::setCheckedIcon(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.checkedIcon)
                    return;
                p.checkedIcon = value;
                _iconUpdate();
            }

            std::string Tool::getText() const
            {
                DJV_PRIVATE_PTR();
                return p.label ? p.label->getText() : std::string();
            }

            void Tool::setText(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (auto context = getContext().lock())
                    {
                        if (!p.label)
                        {
                            p.label = Label::create(context);
                            p.label->setTextHAlign(p.textHAlign);
                            p.label->setFont(p.font);
                            p.label->setFontFace(p.fontFace);
                            p.label->setFontSizeRole(p.fontSizeRole);
                            p.label->setTextColorRole(isChecked() ? ColorRole::Checked : getForegroundColorRole());
                            p.layout->addChild(p.label);
                            p.layout->setStretch(p.label, RowStretch::Expand);
                            p.label->moveToBack();
                        }
                        p.label->setText(value);
                    }
                }
                else
                {
                    p.layout->removeChild(p.label);
                    p.label.reset();
                }
            }

            TextHAlign Tool::getTextHAlign() const
            {
                return _p->textHAlign;
            }

            void Tool::setTextHAlign(TextHAlign value)
            {
                DJV_PRIVATE_PTR();
                p.textHAlign = value;
                if (p.label)
                {
                    p.label->setTextHAlign(value);
                }
            }

            const std::string& Tool::getFont() const
            {
                return _p->font;
            }

            const std::string& Tool::getFontFace() const
            {
                return _p->fontFace;
            }

            MetricsRole Tool::getFontSizeRole() const
            {
                return _p->fontSizeRole;
            }

            void Tool::setFont(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                p.font = value;
                if (p.label)
                {
                    p.label->setFont(value);
                }
            }

            void Tool::setFontFace(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                p.fontFace = value;
                if (p.label)
                {
                    p.label->setFontFace(value);
                }
            }

            void Tool::setFontSizeRole(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                p.fontSizeRole = value;
                if (p.label)
                {
                    p.label->setFontSizeRole(value);
                }
            }

            const Layout::Margin & Tool::getInsideMargin() const
            {
                return _p->border->getMargin();
            }

            void Tool::setInsideMargin(const Layout::Margin & value)
            {
                _p->layout->setMargin(value);
            }

            void Tool::setTextFocusEnabled(bool value)
            {
                if (_p->textFocusEnabled == value)
                    return;
                _p->textFocusEnabled = value;
                if (!_p->textFocusEnabled)
                {
                    releaseTextFocus();
                }
            }

            void Tool::setChecked(bool value)
            {
                IButton::setChecked(value);
                _iconUpdate();
            }

            void Tool::setForegroundColorRole(ColorRole value)
            {
                IButton::setForegroundColorRole(value);
                DJV_PRIVATE_PTR();
                if (p.iconWidget)
                {
                    p.iconWidget->setIconColorRole(value);
                }
                if (p.label)
                {
                    p.label->setTextColorRole(value);
                }
            }

            void Tool::addAction(const std::shared_ptr<Action>& value)
            {
                IButton::addAction(value);
                _actionUpdate();
            }

            void Tool::removeAction(const std::shared_ptr<Action>& value)
            {
                IButton::removeAction(value);
                _actionUpdate();
            }

            void Tool::clearActions()
            {
                IButton::clearActions();
                _actionUpdate();
            }

            bool Tool::acceptFocus(TextFocusDirection)
            {
                bool out = false;
                if (_p->textFocusEnabled && isEnabled(true) && isVisible(true) && !isClipped())
                {
                    takeTextFocus();
                    out = true;
                }
                return out;
            }

            void Tool::_preLayoutEvent(Event::PreLayout & event)
            {
                _setMinimumSize(_p->border->getMinimumSize());
            }

            void Tool::_layoutEvent(Event::Layout &)
            {
                _p->border->setGeometry(getGeometry());
            }

            void Tool::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                const auto& style = _getStyle();
                const BBox2f& g = getGeometry();
                auto render = _getRender();

                if (_isToggled())
                {
                    render->setFillColor(style->getColor(ColorRole::Checked));
                    render->drawRect(g);
                }
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

            void Tool::_buttonPressEvent(Event::ButtonPress& event)
            {
                IButton::_buttonPressEvent(event);
                DJV_PRIVATE_PTR();
                if (event.isAccepted())
                {
                    if (p.textFocusEnabled)
                    {
                        takeTextFocus();
                    }
                }
            }

            void Tool::_keyPressEvent(Event::KeyPress& event)
            {
                IButton::_keyPressEvent(event);
                DJV_PRIVATE_PTR();
                if (!event.isAccepted())
                {
                    switch (event.getKey())
                    {
                    case GLFW_KEY_ENTER:
                    case GLFW_KEY_SPACE:
                        event.accept();
                        switch (getButtonType())
                        {
                        case ButtonType::Push:
                            _doClickedCallback();
                            break;
                        case ButtonType::Toggle:
                        case ButtonType::Radio:
                        case ButtonType::Exclusive:
                            setChecked(!isChecked());
                            _doCheckedCallback(isChecked());
                            break;
                        default: break;
                        }
                        break;
                    case GLFW_KEY_ESCAPE:
                        event.accept();
                        releaseTextFocus();
                        break;
                    default: break;
                    }
                }
            }

            void Tool::_textFocusEvent(Event::TextFocus&)
            {
                _p->border->setBorderColorRole(UI::ColorRole::TextFocus);
            }

            void Tool::_textFocusLostEvent(Event::TextFocusLost&)
            {
                _p->border->setBorderColorRole(UI::ColorRole::None);
            }

            void Tool::_actionUpdate()
            {
                DJV_PRIVATE_PTR();
                const auto& actions = getActions();
                if (actions.size())
                {
                    p.action = actions.front();
                    auto weak = std::weak_ptr<Tool>(std::dynamic_pointer_cast<Tool>(shared_from_this()));
                    p.iconObserver = ValueObserver<std::string>::create(
                        p.action->observeIcon(),
                        [weak](const std::string& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->setIcon(value);
                            }
                        });
                }
                else
                {
                    p.action.reset();
                    p.iconObserver.reset();
                }
            }

            void ToolButton::_iconUpdate()
            {
                DJV_PRIVATE_PTR();
                if (p.iconWidget)
                {
                    p.iconWidget->setIcon(isChecked() && !p.checkedIcon.empty() ? p.checkedIcon : p.icon);
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
