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

#include <djvUI/CheckBox.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/Label.h>
#include <djvUI/Style.h>

#include <djvAV/Render2D.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            struct CheckBox::Private
            {
                std::shared_ptr<Label> label;
            };

            void CheckBox::_init(const std::shared_ptr<Context>& context)
            {
                IButton::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::Button::CheckBox");
                setButtonType(UI::ButtonType::Toggle);

                p.label = Label::create(context);
                p.label->setTextHAlign(TextHAlign::Left);
                p.label->setTextColorRole(getForegroundColorRole());
                p.label->setMargin(MetricsRole::MarginSmall);
                addChild(p.label);
            }

            CheckBox::CheckBox() :
                _p(new Private)
            {}

            CheckBox::~CheckBox()
            {}

            std::shared_ptr<CheckBox> CheckBox::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<CheckBox>(new CheckBox);
                out->_init(context);
                return out;
            }

            std::string CheckBox::getText() const
            {
                DJV_PRIVATE_PTR();
                return p.label ? p.label->getText() : std::string();
            }

            void CheckBox::setText(const std::string & value)
            {
                _p->label->setText(value);
            }

            const std::string & CheckBox::getFont() const
            {
                return _p->label->getFont();
            }

            const std::string & CheckBox::getFontFace() const
            {
                return _p->label->getFontFace();
            }

            MetricsRole CheckBox::getFontSizeRole() const
            {
                return _p->label->getFontSizeRole();
            }

            void CheckBox::setFont(const std::string & value)
            {
                _p->label->setFont(value);
            }

            void CheckBox::setFontFace(const std::string & value)
            {
                _p->label->setFontFace(value);
            }

            void CheckBox::setFontSizeRole(MetricsRole value)
            {
                _p->label->setFontSizeRole(value);
            }

            void CheckBox::_preLayoutEvent(Event::PreLayout & event)
            {
                const auto& style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float is = style->getMetric(MetricsRole::IconSmall);
                glm::vec2 size = _p->label->getMinimumSize();
                size.x += is + m * 2.f;
                size.y = std::max(size.y, is + m * 2.f);
                _setMinimumSize(size);
            }

            void CheckBox::_layoutEvent(Event::Layout &)
            {
                _p->label->setGeometry(_getLabelGeometry());
            }

            void CheckBox::_paintEvent(Event::Paint& event)
            {
                IButton::_paintEvent(event);
                auto render = _getRender();
                const auto& style = _getStyle();
                const BBox2f& g = getGeometry();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float b = style->getMetric(MetricsRole::Border);
                const float is = style->getMetric(MetricsRole::IconSmall);

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

                BBox2f checkGeometry = _getCheckGeometry().margin(-m);
                render->setFillColor(style->getColor(ColorRole::Border));
                drawBorder(render, checkGeometry, b);
                if (_isToggled())
                {
                    render->setFillColor(style->getColor(ColorRole::Checked));
                    render->drawRect(checkGeometry.margin(-b));
                }
            }

            BBox2f CheckBox::_getCheckGeometry() const
            {
                const auto& style = _getStyle();
                const BBox2f& g = getGeometry();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float is = style->getMetric(MetricsRole::IconSmall);
                return BBox2f(g.min.x, g.min.y, is + m * 2.f, g.h());
            }

            BBox2f CheckBox::_getLabelGeometry() const
            {
                const auto& style = _getStyle();
                const BBox2f& g = getGeometry();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float is = style->getMetric(MetricsRole::IconSmall);
                return BBox2f(g.min.x + is + m * 2.f, g.min.y, g.w() - (is + m * 2.f), g.h());
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
