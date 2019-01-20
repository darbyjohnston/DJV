//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvUI/RadioButton.h>

#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2D.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            struct Radio::Private
            {
                std::shared_ptr<Icon> icon;
                std::shared_ptr<Label> label;
                std::shared_ptr<Layout::Horizontal> layout;
            };

            void Radio::_init(Context * context)
            {
                IButton::_init(context);

                setClassName("djv::UI::Button::Radio");

                DJV_PRIVATE_PTR();
                p.icon = Icon::create(context);
                p.icon->setVAlign(VAlign::Center);
                p.icon->hide();

                p.label = Label::create(context);
                p.label->hide();

                p.layout = Layout::Horizontal::create(context);
                p.layout->setMargin(Style::MetricsRole::MarginSmall);
                p.layout->addWidget(p.icon);
                p.layout->addWidget(p.label, Layout::RowStretch::Expand);
                p.layout->setParent(shared_from_this());
            }

            Radio::Radio() :
                _p(new Private)
            {}

            Radio::~Radio()
            {}

            std::shared_ptr<Radio> Radio::create(Context * context)
            {
                auto out = std::shared_ptr<Radio>(new Radio);
                out->_init(context);
                return out;
            }

            std::shared_ptr<Radio> Radio::create(const std::string& text, Context * context)
            {
                auto out = std::shared_ptr<Radio>(new Radio);
                out->_init(context);
                out->setText(text);
                return out;
            }

            std::shared_ptr<Radio> Radio::create(const std::string& text, const std::string& icon, Context * context)
            {
                auto out = std::shared_ptr<Radio>(new Radio);
                out->_init(context);
                out->setIcon(icon);
                out->setText(text);
                return out;
            }

            const std::string& Radio::getIcon() const
            {
                return _p->icon->getIcon();
            }

            void Radio::setIcon(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.icon->setIcon(value);
                p.icon->setVisible(!value.empty());
            }

            const std::string& Radio::getText() const
            {
                return _p->label->getText();
            }

            void Radio::setText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.label->setText(value);
                p.label->setVisible(!value.empty());
            }

            TextHAlign Radio::getTextHAlign() const
            {
                return _p->label->getTextHAlign();
            }

            TextVAlign Radio::getTextVAlign() const
            {
                return _p->label->getTextVAlign();
            }

            void Radio::setTextHAlign(TextHAlign value)
            {
                _p->label->setTextHAlign(value);
            }

            void Radio::setTextVAlign(TextVAlign value)
            {
                _p->label->setTextVAlign(value);
            }

            const std::string & Radio::getFont() const
            {
                return _p->label->getFont();
            }

            const std::string & Radio::getFontFace() const
            {
                return _p->label->getFontFace();
            }

            Style::MetricsRole Radio::getFontSizeRole() const
            {
                return _p->label->getFontSizeRole();
            }

            void Radio::setFont(const std::string & value)
            {
                _p->label->setFont(value);
            }

            void Radio::setFontFace(const std::string & value)
            {
                _p->label->setFontFace(value);
            }

            void Radio::setFontSizeRole(Style::MetricsRole value)
            {
                _p->label->setFontSizeRole(value);
            }

            const Layout::Margin& Radio::getInsideMargin() const
            {
                return _p->layout->getMargin();
            }

            void Radio::setInsideMargin(const Layout::Margin& value)
            {
                _p->layout->setMargin(value);
            }

            float Radio::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }

            void Radio::_preLayoutEvent(Event::PreLayout& event)
            {
                if (auto style = _getStyle().lock())
                {
                    const float ms = style->getMetric(Style::MetricsRole::MarginSmall);
                    glm::vec2 size = _p->layout->getMinimumSize();
                    size.y += ms;
                    _setMinimumSize(size);
                }
            }

            void Radio::_layoutEvent(Event::Layout&)
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f & g = getGeometry();
                    const float ms = style->getMetric(Style::MetricsRole::MarginSmall);
                    BBox2f layoutRect = g;
                    layoutRect.max.y -= ms;
                    _p->layout->setGeometry(layoutRect);
                }
            }

            void Radio::_paintEvent(Event::Paint& event)
            {
                if (auto render = _getRender().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const BBox2f& g = getGeometry();
                        const float ms = style->getMetric(Style::MetricsRole::MarginSmall);

                        // Draw the background.
                        BBox2f bgRect = g;
                        bgRect.max.y -= ms;
                        render->setFillColor(_getColorWithOpacity(style->getColor(getBackgroundRole())));
                        render->drawRect(bgRect);

                        // Draw the toggled state.
                        if (_isToggled())
                        {
                            BBox2f toggledRect = g;
                            toggledRect.min.y = g.max.y - ms;
                            render->setFillColor(_getColorWithOpacity(style->getColor(getCheckedColorRole())));
                            render->drawRect(toggledRect);
                        }

                        // Draw the hovered state.
                        if (_isHovered())
                        {
                            render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Hover)));
                            render->drawRect(g);
                        }
                    }
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
