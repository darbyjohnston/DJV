// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ToolButton.h>

#include <djvUI/Action.h>
#include <djvUI/DrawUtil.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

#include <djvSystem/Timer.h>

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
            namespace
            {
                //! \todo Should this be configurable?
                const size_t autoRepeatStart = 500;
                const size_t autoRepeatTimeout = 150;

            } // namespace

            struct Tool::Private
            {
                std::string icon;
                std::string checkedIcon;
                MetricsRole iconSizeRole = MetricsRole::Icon;
                std::shared_ptr<Icon> iconWidget;
                std::shared_ptr<Label> label;
                TextHAlign textHAlign = TextHAlign::Left;
                std::string font;
                std::string fontFace;
                MetricsRole fontSizeRole = MetricsRole::FontMedium;
                size_t elide = 0;
                MetricsRole insideMargin = MetricsRole::MarginInside;
                std::shared_ptr<Action> action;
                bool textFocusEnabled = false;
                bool autoRepeat = false;
                std::shared_ptr<System::Timer> autoRepeatStartTimer;
                std::shared_ptr<System::Timer> autoRepeatTimer;
                std::shared_ptr<ValueObserver<std::string> > iconObserver;
            };

            void Tool::_init(const std::shared_ptr<System::Context>& context)
            {
                IButton::_init(context);

                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Button::Tool");

                p.autoRepeatStartTimer = System::Timer::create(context);
                p.autoRepeatTimer = System::Timer::create(context);
                p.autoRepeatTimer->setRepeating(true);

                _actionUpdate();
                _iconUpdate();
            }

            Tool::Tool() :
                _p(new Private)
            {}

            Tool::~Tool()
            {}

            std::shared_ptr<Tool> Tool::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Tool>(new Tool);
                out->_init(context);
                return out;
            }

            void Tool::setIcon(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.icon)
                    return;
                p.icon = value;
                _iconUpdate();
            }
            
            void Tool::setIconSizeRole(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.iconSizeRole)
                    return;
                p.iconSizeRole = value;
                _iconUpdate();
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

            void Tool::setText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (!p.label)
                    {
                        if (auto context = getContext().lock())
                        {
                            p.label = Label::create(context);
                            p.label->setTextHAlign(p.textHAlign);
                            p.label->setTextColorRole(isChecked() ? ColorRole::Checked : getForegroundColorRole());
                            p.label->setFontFamily(p.font);
                            p.label->setFontFace(p.fontFace);
                            p.label->setFontSizeRole(p.fontSizeRole);
                            p.label->setElide(p.elide);
                            addChild(p.label);
                        }
                    }
                    if (p.label)
                    {
                        p.label->setText(value);
                    }
                }
                else
                {
                    removeChild(p.label);
                    p.label.reset();
                    _resize();
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

            void Tool::setFont(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.font = value;
                if (p.label)
                {
                    p.label->setFontFamily(value);
                }
            }

            void Tool::setFontFace(const std::string& value)
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

            size_t Tool::getElide() const
            {
                return _p->elide;
            }

            void Tool::setElide(size_t value)
            {
                DJV_PRIVATE_PTR();
                p.elide = value;
                if (p.label)
                {
                    p.label->setElide(value);
                }
            }

            MetricsRole Tool::getInsideMargin() const
            {
                return _p->insideMargin;
            }

            void Tool::setInsideMargin(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.insideMargin)
                    return;
                p.insideMargin = value;
                _resize();
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
            
            void Tool::setAutoRepeat(bool value)
            {
                _p->autoRepeat = value;
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

            void Tool::_doClick()
            {
                DJV_PRIVATE_PTR();
                if (p.action)
                {
                    p.action->doClick();
                }
                else
                {
                    IButton::_doClick();
                }
            }

            void Tool::_doCheck(bool value)
            {
                DJV_PRIVATE_PTR();
                if (!p.action)
                {
                    IButton::_doCheck(value);
                }
            }

            void Tool::_preLayoutEvent(System::Event::PreLayout& event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float m = style->getMetric(p.insideMargin);
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                glm::vec2 size = glm::vec2(0.F, 0.F);
                if (p.iconWidget)
                {
                    const auto& tmp = p.iconWidget->getMinimumSize();
                    size.x += tmp.x;
                    size.y = std::max(size.y, tmp.y);
                }
                if (p.label)
                {
                    const auto& tmp = p.label->getMinimumSize();
                    size.x += tmp.x;
                    size.y = std::max(size.y, tmp.y);
                }
                size += m * 2.F;
                if (p.textFocusEnabled)
                {
                    size += btf * 2.F;
                }
                _setMinimumSize(size + getMargin().getSize(style));
            }

            void Tool::_layoutEvent(System::Event::Layout&)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                const float m = style->getMetric(p.insideMargin);
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                Math::BBox2f g2 = g;
                if (p.textFocusEnabled)
                {
                    g2 = g2.margin(-btf);
                }
                g2 = g2.margin(-m);
                float x = g2.min.x;
                float y = g2.min.y + g2.h() / 2.F;
                float w = g2.w();
                if (p.iconWidget)
                {
                    const auto& tmp = p.iconWidget->getMinimumSize();
                    p.iconWidget->setGeometry(Math::BBox2f(x, floorf(y - tmp.y / 2.F), tmp.x, tmp.y));
                    x += tmp.x;
                    w -= tmp.x;
                }
                if (p.label)
                {
                    const auto& tmp = p.label->getMinimumSize();
                    p.label->setGeometry(Math::BBox2f(x, floorf(y - tmp.y / 2.F), w, tmp.y));
                }
            }

            void Tool::_paintEvent(System::Event::Paint& event)
            {
                Widget::_paintEvent(event);
                const auto& style = _getStyle();
                const Math::BBox2f& g = getGeometry();
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                const auto& render = _getRender();

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

                if (hasTextFocus())
                {
                    render->setFillColor(style->getColor(ColorRole::TextFocus));
                    drawBorder(render, g, btf);
                }
            }

            void Tool::_buttonPressEvent(System::Event::ButtonPress& event)
            {
                IButton::_buttonPressEvent(event);
                DJV_PRIVATE_PTR();
                if (event.isAccepted())
                {
                    if (p.textFocusEnabled)
                    {
                        takeTextFocus();
                    }
                    if (p.autoRepeat)
                    {
                        auto weak = std::weak_ptr<Tool>(std::dynamic_pointer_cast<Tool>(shared_from_this()));
                        p.autoRepeatStartTimer->start(
                            std::chrono::milliseconds(autoRepeatStart),
                            [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                            {
                                if (auto widget = weak.lock())
                                {
                                    widget->_p->autoRepeatTimer->start(
                                        std::chrono::milliseconds(autoRepeatTimeout),
                                        [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                                        {
                                            if (auto widget2 = weak.lock())
                                            {
                                                widget2->_doClick();
                                            }
                                        });
                                }
                            });
                    }
                }
            }

            void Tool::_buttonReleaseEvent(System::Event::ButtonRelease& event)
            {
                IButton::_buttonReleaseEvent(event);
                DJV_PRIVATE_PTR();
                if (event.isAccepted())
                {
                    p.autoRepeatStartTimer->stop();
                    p.autoRepeatTimer->stop();
                }
            }

            void Tool::_keyPressEvent(System::Event::KeyPress& event)
            {
                IButton::_keyPressEvent(event);
                if (!event.isAccepted() && hasTextFocus())
                {
                    switch (event.getKey())
                    {
                    case GLFW_KEY_ENTER:
                    case GLFW_KEY_SPACE:
                        event.accept();
                        switch (getButtonType())
                        {
                        case ButtonType::Push:
                            _doClick();
                            break;
                        case ButtonType::Toggle:
                        case ButtonType::Radio:
                        case ButtonType::Exclusive:
                            _doCheck(!isChecked());
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

            void Tool::_textFocusEvent(System::Event::TextFocus&)
            {
                _redraw();
            }

            void Tool::_textFocusLostEvent(System::Event::TextFocusLost&)
            {
                _redraw();
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
                const std::string& icon = isChecked() && !p.checkedIcon.empty() ? p.checkedIcon : p.icon;
                if (!icon.empty())
                {
                    if (!p.iconWidget)
                    {
                        if (auto context = getContext().lock())
                        {
                            p.iconWidget = Icon::create(context);
                            addChild(p.iconWidget);
                        }
                    }
                    p.iconWidget->setIcon(icon);
                    p.iconWidget->setIconSizeRole(p.iconSizeRole);
                }
                else
                {
                    removeChild(p.iconWidget);
                    p.iconWidget.reset();
                    _resize();
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
