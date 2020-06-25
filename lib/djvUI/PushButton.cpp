// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/PushButton.h>

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
            struct Push::Private
            {
                std::shared_ptr<Icon> icon;
                std::shared_ptr<Label> label;
                TextHAlign textHAlign = TextHAlign::Center;
                ColorRole textColorRole = ColorRole::Foreground;
                std::string font;
                std::string fontFace;
                MetricsRole fontSizeRole = MetricsRole::FontMedium;
                std::shared_ptr<HorizontalLayout> layout;
            };

            void Push::_init(const std::shared_ptr<Context>& context)
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

            std::shared_ptr<Push> Push::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Push>(new Push);
                out->_init(context);
                return out;
            }

            std::string Push::getIcon() const
            {
                DJV_PRIVATE_PTR();
                return p.icon ? p.icon->getIcon() : std::string();
            }

            void Push::setIcon(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (auto context = getContext().lock())
                    {
                        if (!p.icon)
                        {
                            p.icon = Icon::create(context);
                            p.icon->setVAlign(VAlign::Center);
                            p.icon->setIconColorRole(getForegroundColorRole());
                            p.layout->addChild(p.icon);
                            p.icon->moveToFront();
                        }
                        p.icon->setIcon(value);
                    }
                }
                else
                {
                    p.layout->removeChild(p.icon);
                    p.icon.reset();
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
                            p.label = Label::create(context);
                            p.label->setTextHAlign(p.textHAlign);
                            p.label->setTextColorRole(getForegroundColorRole());
                            p.label->setFontFamily(p.font);
                            p.label->setFontFace(p.fontFace);
                            p.label->setFontSizeRole(p.fontSizeRole);
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

            TextHAlign Push::getTextHAlign() const
            {
                return _p->textHAlign;
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

            const Layout::Margin& Push::getInsideMargin() const
            {
                return _p->layout->getMargin();
            }

            void Push::setInsideMargin(const Layout::Margin& value)
            {
                _p->layout->setMargin(value);
            }

            void Push::setForegroundColorRole(ColorRole value)
            {
                IButton::setForegroundColorRole(value);
                if (_p->icon)
                {
                    _p->icon->setIconColorRole(isChecked() ? ColorRole::Checked : value);
                }
                if (_p->label)
                {
                    _p->label->setTextColorRole(value);
                }
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

            void Push::_preLayoutEvent(Event::PreLayout& event)
            {
                const auto& style = _getStyle();
                const float b = style->getMetric(MetricsRole::Border);
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                glm::vec2 size = _p->layout->getMinimumSize();
                _setMinimumSize(size + b * 2.F + btf * 2.F);
            }

            void Push::_layoutEvent(Event::Layout&)
            {
                const auto& style = _getStyle();
                const float b = style->getMetric(MetricsRole::Border);
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                const BBox2f& g = getGeometry();
                _p->layout->setGeometry(g.margin(-(b + btf)));
            }

            void Push::_paintEvent(Event::Paint& event)
            {
                const auto& style = _getStyle();
                const float b = style->getMetric(MetricsRole::Border);
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                const auto& render = _getRender();
                if (hasTextFocus())
                {
                    render->setFillColor(style->getColor(ColorRole::TextFocus));
                    drawBorder(render, g, btf);
                }
                const BBox2f& g2 = g.margin(-btf);
                render->setFillColor(style->getColor(ColorRole::BorderButton));
                drawBorder(render, g2, b);
                const BBox2f& g3 = g2.margin(-b);
                if (_isToggled())
                {
                    render->setFillColor(style->getColor(ColorRole::Checked));
                    render->drawRect(g3);
                }
                else
                {
                    render->setFillColor(style->getColor(ColorRole::Button));
                    render->drawRect(g3);
                }
                if (_isPressed())
                {
                    render->setFillColor(style->getColor(ColorRole::Pressed));
                    render->drawRect(g3);
                }
                else if (_isHovered())
                {
                    render->setFillColor(style->getColor(ColorRole::Hovered));
                    render->drawRect(g3);
                }
            }

            void Push::_buttonPressEvent(Event::ButtonPress& event)
            {
                IButton::_buttonPressEvent(event);
                if (event.isAccepted())
                {
                    takeTextFocus();
                }
            }

            void Push::_keyPressEvent(Event::KeyPress& event)
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

            void Push::_textFocusEvent(Event::TextFocus&)
            {
                _redraw();
            }

            void Push::_textFocusLostEvent(Event::TextFocusLost&)
            {
                _redraw();
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
