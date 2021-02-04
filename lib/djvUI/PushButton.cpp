// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/PushButton.h>

#include <djvUI/Action.h>
#include <djvUI/DrawUtil.h>
#include <djvUI/IconWidget.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>

#include <djvRender2D/Render.h>

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
            struct Push::Private
            {
                std::shared_ptr<IconWidget> iconWidget;
                std::shared_ptr<Text::Label> label;
                std::string font;
                std::string fontFace;
                MetricsRole fontSizeRole = MetricsRole::FontMedium;
                TextHAlign textHAlign = TextHAlign::Center;
                ColorRole textColorRole = ColorRole::Foreground;
                std::shared_ptr<Action> action;
                std::shared_ptr<HorizontalLayout> layout;
                std::shared_ptr<Observer::Value<std::string> > iconObserver;
                std::shared_ptr<Observer::Value<std::string> > textObserver;
            };

            void Push::_init(const std::shared_ptr<System::Context>& context)
            {
                IButton::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::Button::Push");
                setVAlign(VAlign::Center);

                p.layout = HorizontalLayout::create(context);
                p.layout->setMargin(MetricsRole::MarginSmall);
                addChild(p.layout);
            }

            Push::Push() :
                _p(new Private)
            {}

            Push::~Push()
            {}

            std::shared_ptr<Push> Push::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Push>(new Push);
                out->_init(context);
                return out;
            }

            std::string Push::getIcon() const
            {
                DJV_PRIVATE_PTR();
                return p.iconWidget ? p.iconWidget->getIcon() : std::string();
            }

            void Push::setIcon(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (auto context = getContext().lock())
                    {
                        if (!p.iconWidget)
                        {
                            p.iconWidget = IconWidget::create(context);
                            p.iconWidget->setVAlign(VAlign::Center);
                            p.iconWidget->setIconColorRole(getForegroundColorRole());
                            p.layout->addChild(p.iconWidget);
                            p.iconWidget->moveToFront();
                        }
                        p.iconWidget->setIcon(value);
                    }
                }
                else
                {
                    p.layout->removeChild(p.iconWidget);
                    p.iconWidget.reset();
                }
            }

            std::string Push::getText() const
            {
                DJV_PRIVATE_PTR();
                return p.label ? p.label->getText() : std::string();
            }

            void Push::setText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (auto context = getContext().lock())
                    {
                        if (!p.label)
                        {
                            p.label = Text::Label::create(context);
                            p.label->setTextHAlign(p.textHAlign);
                            p.label->setTextColorRole(getForegroundColorRole());
                            p.label->setFontFamily(p.font);
                            p.label->setFontFace(p.fontFace);
                            p.label->setFontSizeRole(p.fontSizeRole);
                            p.layout->addChild(p.label);
                            p.layout->setStretch(p.label);
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

            const std::string& Push::getFont() const
            {
                return _p->font;
            }

            const std::string& Push::getFontFace() const
            {
                return _p->fontFace;
            }

            MetricsRole Push::getFontSizeRole() const
            {
                return _p->fontSizeRole;
            }

            void Push::setFont(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.font = value;
                if (p.label)
                {
                    p.label->setFontFamily(value);
                }
            }

            void Push::setFontFace(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.fontFace = value;
                if (p.label)
                {
                    p.label->setFontFace(value);
                }
            }

            void Push::setFontSizeRole(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                p.fontSizeRole = value;
                if (p.label)
                {
                    p.label->setFontSizeRole(value);
                }
            }

            TextHAlign Push::getTextHAlign() const
            {
                return _p->textHAlign;
            }

            const Layout::Margin& Push::getInsideMargin() const
            {
                return _p->layout->getMargin();
            }

            void Push::setTextHAlign(TextHAlign value)
            {
                DJV_PRIVATE_PTR();
                p.textHAlign = value;
                if (p.label)
                {
                    p.label->setTextHAlign(value);
                }
            }

            void Push::setInsideMargin(const Layout::Margin& value)
            {
                _p->layout->setMargin(value);
            }

            void Push::setForegroundColorRole(ColorRole value)
            {
                IButton::setForegroundColorRole(value);
                if (_p->iconWidget)
                {
                    _p->iconWidget->setIconColorRole(isChecked() ? ColorRole::Checked : value);
                }
                if (_p->label)
                {
                    _p->label->setTextColorRole(value);
                }
            }

            void Push::addAction(const std::shared_ptr<Action>& value)
            {
                IButton::addAction(value);
                _actionUpdate();
            }

            void Push::removeAction(const std::shared_ptr<Action>& value)
            {
                IButton::removeAction(value);
                _actionUpdate();
            }

            void Push::clearActions()
            {
                IButton::clearActions();
                _actionUpdate();
            }

            bool Push::acceptFocus(TextFocusDirection)
            {
                bool out = false;
                if (isEnabled(true) && isVisible(true) && !isClipped())
                {
                    takeTextFocus();
                    out = true;
                }
                return out;
            }

            void Push::_doClick()
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

            void Push::_doCheck(bool value)
            {
                DJV_PRIVATE_PTR();
                if (!p.action)
                {
                    IButton::_doCheck(value);
                }
            }

            void Push::_preLayoutEvent(System::Event::PreLayout& event)
            {
                const auto& style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float b = style->getMetric(MetricsRole::Border);
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                glm::vec2 size = _p->layout->getMinimumSize();
                size.x += m * 2.F;
                _setMinimumSize(size + b * 2.F + btf * 2.F);
            }

            void Push::_layoutEvent(System::Event::Layout&)
            {
                const auto& style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float b = style->getMetric(MetricsRole::Border);
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                const Math::BBox2f& g = getGeometry().margin(-b - btf);
                _p->layout->setGeometry(Math::BBox2f(g.min.x + m, g.min.y, g.w() - m * 2.F, g.h()));
            }

            void Push::_paintEvent(System::Event::Paint& event)
            {
                const auto& style = _getStyle();
                const float b = style->getMetric(MetricsRole::Border);
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                const auto& render = _getRender();
                if (hasTextFocus())
                {
                    render->setFillColor(style->getColor(ColorRole::TextFocus));
                    drawBorder(render, g, btf);
                }
                Math::BBox2f g2 = g.margin(-btf);

                render->setFillColor(style->getColor(ColorRole::Border));
                drawBorder(render, g2, b);
                g2 = g2.margin(-b);

                if (_isToggled())
                {
                    render->setFillColor(style->getColor(ColorRole::Checked));
                    render->drawRect(g2);
                }
                else
                {
                    render->setFillColor(style->getColor(ColorRole::Button));
                    render->drawRect(g2);
                }
                if (_isPressed())
                {
                    render->setFillColor(style->getColor(ColorRole::Pressed));
                    render->drawRect(g2);
                }
                else if (_isHovered())
                {
                    render->setFillColor(style->getColor(ColorRole::Hovered));
                    render->drawRect(g2);
                }
            }

            void Push::_buttonPressEvent(System::Event::ButtonPress& event)
            {
                IButton::_buttonPressEvent(event);
                if (event.isAccepted())
                {
                    takeTextFocus();
                }
            }

            void Push::_keyPressEvent(System::Event::KeyPress& event)
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

            void Push::_textFocusEvent(System::Event::TextFocus&)
            {
                _redraw();
            }

            void Push::_textFocusLostEvent(System::Event::TextFocusLost&)
            {
                _redraw();
            }

            void Push::_actionUpdate()
            {
                DJV_PRIVATE_PTR();
                const auto& actions = getActions();
                if (actions.size())
                {
                    p.action = actions.front();
                    auto weak = std::weak_ptr<Push>(std::dynamic_pointer_cast<Push>(shared_from_this()));
                    p.iconObserver = Observer::Value<std::string>::create(
                        p.action->observeIcon(),
                        [weak](const std::string& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->setIcon(value);
                            }
                        });
                    p.textObserver = Observer::Value<std::string>::create(
                        p.action->observeTextBrief(),
                        [weak](const std::string& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->setText(value);
                            }
                        });
                }
                else
                {
                    p.action.reset();
                    p.iconObserver.reset();
                    p.textObserver.reset();
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
