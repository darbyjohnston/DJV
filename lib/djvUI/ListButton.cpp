// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ListButton.h>

#include <djvUI/Action.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>

#include <djvRender2D/Render.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            struct List::Private
            {
                std::shared_ptr<Icon> icon;
                std::shared_ptr<Icon> rightIcon;
                std::shared_ptr<Text::Label> label;
                std::shared_ptr<Text::Label> rightLabel;
                TextHAlign textHAlign = TextHAlign::Left;
                std::string font;
                std::string fontFace;
                MetricsRole fontSizeRole = MetricsRole::FontMedium;
                size_t textElide = 0;
                std::shared_ptr<HorizontalLayout> layout;
            };

            void List::_init(const std::shared_ptr<System::Context>& context)
            {
                IButton::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::Button::List");

                p.layout = HorizontalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                addChild(p.layout);
            }

            List::List() :
                _p(new Private)
            {}

            List::~List()
            {}

            std::shared_ptr<List> List::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<List>(new List);
                out->_init(context);
                return out;
            }

            std::string List::getIcon() const
            {
                DJV_PRIVATE_PTR();
                return p.icon ? p.icon->getIcon() : std::string();
            }

            std::string List::getRightIcon() const
            {
                DJV_PRIVATE_PTR();
                return p.rightIcon ? p.rightIcon->getIcon() : std::string();
            }

            void List::setIcon(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (!p.icon)
                    {
                        if (auto context = getContext().lock())
                        {
                            p.icon = Icon::create(context);
                            p.icon->setVAlign(VAlign::Center);
                            _widgetUpdate();
                        }
                    }
                    p.icon->setIcon(value);
                }
                else
                {
                    p.layout->removeChild(p.icon);
                    p.icon.reset();
                }
            }

            void List::setRightIcon(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (!p.rightIcon)
                    {
                        if (auto context = getContext().lock())
                        {
                            p.rightIcon = Icon::create(context);
                            p.rightIcon->setVAlign(VAlign::Center);
                            _widgetUpdate();
                        }
                    }
                    p.rightIcon->setIcon(value);
                }
                else
                {
                    p.layout->removeChild(p.rightIcon);
                    p.rightIcon.reset();
                }
            }

            std::string List::getText() const
            {
                DJV_PRIVATE_PTR();
                return p.label ? p.label->getText() : std::string();
            }

            std::string List::getRightText() const
            {
                DJV_PRIVATE_PTR();
                return p.rightLabel ? p.rightLabel->getText() : std::string();
            }

            void List::setText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (!p.label)
                    {
                        if (auto context = getContext().lock())
                        {
                            p.label = Text::Label::create(context);
                            _widgetUpdate();
                        }
                    }
                    p.label->setText(value);
                }
                else
                {
                    p.layout->removeChild(p.label);
                    p.label.reset();
                }
            }

            void List::setRightText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (!p.rightLabel)
                    {
                        if (auto context = getContext().lock())
                        {
                            p.rightLabel = Text::Label::create(context);
                            _widgetUpdate();
                        }
                    }
                    p.rightLabel->setText(value);
                }
                else
                {
                    p.layout->removeChild(p.rightLabel);
                    p.rightLabel.reset();
                }
            }

            const std::string& List::getFont() const
            {
                return _p->font;
            }

            const std::string& List::getFontFace() const
            {
                return _p->fontFace;
            }

            MetricsRole List::getFontSizeRole() const
            {
                return _p->fontSizeRole;
            }

            void List::setFont(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.font)
                    return;
                p.font = value;
                _widgetUpdate();
            }

            void List::setFontFace(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.fontFace)
                    return;
                p.fontFace = value;
                _widgetUpdate();
            }

            void List::setFontSizeRole(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.fontSizeRole)
                    return;
                p.fontSizeRole = value;
                _widgetUpdate();
            }

            TextHAlign List::getTextHAlign() const
            {
                return _p->textHAlign;
            }

            size_t List::getTextElide() const
            {
                return _p->textElide;
            }

            const Layout::Margin& List::getInsideMargin() const
            {
                return _p->layout->getMargin();
            }

            void List::setTextHAlign(TextHAlign value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.textHAlign)
                    return;
                p.textHAlign = value;
                _widgetUpdate();
            }

            void List::setTextElide(size_t value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.textElide)
                    return;
                p.textElide = value;
                _widgetUpdate();
            }

            void List::setInsideMargin(const Layout::Margin& value)
            {
                _p->layout->setMargin(value);
            }

            void List::setForegroundColorRole(ColorRole value)
            {
                IButton::setForegroundColorRole(value);
                _widgetUpdate();
            }

            void List::_preLayoutEvent(System::Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void List::_layoutEvent(System::Event::Layout&)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void List::_paintEvent(System::Event::Paint& event)
            {
                IButton::_paintEvent(event);
                const auto& render = _getRender();
                const auto& style = _getStyle();
                const Math::BBox2f& g = getGeometry();
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

            void List::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                const ColorRole foregroundColorRole = getForegroundColorRole();
                p.layout->clearChildren();
                if (p.icon)
                {
                    p.icon->setIconColorRole(foregroundColorRole);
                    p.layout->addChild(p.icon);
                }
                if (p.label)
                {
                    p.label->setTextHAlign(p.textHAlign);
                    p.label->setTextColorRole(foregroundColorRole);
                    p.label->setFontFamily(p.font);
                    p.label->setFontFace(p.fontFace);
                    p.label->setFontSizeRole(p.fontSizeRole);
                    p.label->setTextElide(p.textElide);
                    p.label->setMargin(MetricsRole::MarginSmall);
                    p.layout->addChild(p.label);
                    p.layout->setStretch(p.label, RowStretch::Expand);
                }
                if (p.rightLabel)
                {
                    p.rightLabel->setTextColorRole(foregroundColorRole);
                    p.rightLabel->setFontFamily(p.font);
                    p.rightLabel->setFontFace(p.fontFace);
                    p.rightLabel->setFontSizeRole(p.fontSizeRole);
                    p.rightLabel->setTextElide(p.textElide);
                    p.rightLabel->setMargin(MetricsRole::MarginSmall);
                    p.layout->addChild(p.rightLabel);
                }
                if (p.rightIcon)
                {
                    p.rightIcon->setIconColorRole(foregroundColorRole);
                    p.layout->addChild(p.rightIcon);
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
