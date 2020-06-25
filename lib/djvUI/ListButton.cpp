// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ListButton.h>

#include <djvUI/Action.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>

#include <djvAV/Render2D.h>

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
                std::shared_ptr<Label> label;
                std::shared_ptr<Label> rightsideLabel;
                TextHAlign textHAlign = TextHAlign::Left;
                std::string font;
                std::string fontFace;
                MetricsRole fontSizeRole = MetricsRole::FontMedium;
                std::shared_ptr<HorizontalLayout> layout;
            };

            void List::_init(const std::shared_ptr<Context>& context)
            {
                IButton::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::Button::List");

                p.layout = HorizontalLayout::create(context);
                p.layout->setMargin(MetricsRole::MarginSmall);
                addChild(p.layout);
            }

            List::List() :
                _p(new Private)
            {}

            List::~List()
            {}

            std::shared_ptr<List> List::create(const std::shared_ptr<Context>& context)
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
                            p.layout->addChild(p.icon);
                            p.icon->moveToFront();
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

            std::string List::getText() const
            {
                DJV_PRIVATE_PTR();
                return p.label ? p.label->getText() : std::string();
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
                            p.label = Label::create(context);
                            p.layout->addChild(p.label);
                            p.layout->setStretch(p.label, RowStretch::Expand);
                            p.label->moveToBack();
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

            TextHAlign List::getTextHAlign() const
            {
                return _p->textHAlign;
            }

            void List::setTextHAlign(TextHAlign value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.textHAlign)
                    return;
                p.textHAlign = value;
                _widgetUpdate();
            }

            std::string List::getRightsideText() const
            {
                DJV_PRIVATE_PTR();
                return p.rightsideLabel ? p.rightsideLabel->getText() : std::string();
            }

            void List::setRightsideText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (!p.rightsideLabel)
                    {
                        if (auto context = getContext().lock())
                        {
                            p.rightsideLabel = Label::create(context);
                            p.layout->addChild(p.rightsideLabel);
                            p.rightsideLabel->moveToFront();
                            _widgetUpdate();
                        }
                    }
                    p.rightsideLabel->setText(value);
                }
                else
                {
                    p.layout->removeChild(p.rightsideLabel);
                    p.rightsideLabel.reset();
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

            const Layout::Margin& List::getInsideMargin() const
            {
                return _p->layout->getMargin();
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

            void List::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void List::_layoutEvent(Event::Layout&)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void List::_paintEvent(Event::Paint& event)
            {
                IButton::_paintEvent(event);
                const auto& render = _getRender();
                const auto& style = _getStyle();
                const BBox2f& g = getGeometry();
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
                if (p.icon)
                {
                    _p->icon->setIconColorRole(isChecked() ? ColorRole::Checked : foregroundColorRole);
                }
                if (p.label)
                {
                    p.label->setTextHAlign(p.textHAlign);
                    p.label->setTextColorRole(getForegroundColorRole());
                    p.label->setFontFamily(p.font);
                    p.label->setFontFace(p.fontFace);
                    p.label->setFontSizeRole(p.fontSizeRole);
                    p.label->setTextColorRole(foregroundColorRole);
                }
                if (p.rightsideLabel)
                {
                    p.rightsideLabel->setTextColorRole(getForegroundColorRole());
                    p.rightsideLabel->setFontFamily(p.font);
                    p.rightsideLabel->setFontFace(p.fontFace);
                    p.rightsideLabel->setFontSizeRole(p.fontSizeRole);
                    p.rightsideLabel->setTextColorRole(foregroundColorRole);
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
