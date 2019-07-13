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
                std::shared_ptr<HorizontalLayout> layout;
            };

            void Tool::_init(Context * context)
            {
                IButton::_init(context);

                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Button::Tool");
                setVAlign(VAlign::Center);

                p.layout = HorizontalLayout::create(context);
                p.layout->setMargin(MetricsRole::MarginSmall);
                addChild(p.layout);
            }

            Tool::Tool() :
                _p(new Private)
            {}

            Tool::~Tool()
            {}

            std::shared_ptr<Tool> Tool::create(Context * context)
            {
                auto out = std::shared_ptr<Tool>(new Tool);
                out->_init(context);
                return out;
            }

            std::shared_ptr<Tool> Tool::create(const std::string & text, Context * context)
            {
                auto out = std::shared_ptr<Tool>(new Tool);
                out->_init(context);
                out->setText(text);
                return out;
            }

            std::shared_ptr<Tool> Tool::create(const std::string & text, const std::string & icon, Context * context)
            {
                auto out = std::shared_ptr<Tool>(new Tool);
                out->_init(context);
                out->setIcon(icon);
                out->setText(text);
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
                    if (!p.iconWidget)
                    {
                        p.iconWidget = Icon::create(getContext());
                        p.iconWidget->setVAlign(VAlign::Center);
                        p.iconWidget->setIconColorRole(isChecked() ? ColorRole::Checked : getForegroundColorRole());
                        p.layout->addChild(p.iconWidget);
                        p.iconWidget->moveToFront();
                    }
                    _iconUpdate();
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
                    if (!p.label)
                    {
                        p.label = Label::create(getContext());
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
                return _p->layout->getMargin();
            }

            void Tool::setInsideMargin(const Layout::Margin & value)
            {
                _p->layout->setMargin(value);
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

            void Tool::_preLayoutEvent(Event::PreLayout & event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void Tool::_layoutEvent(Event::Layout &)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void Tool::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                const BBox2f& g = getGeometry();
                auto render = _getRender();
                const auto& style = _getStyle();
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
