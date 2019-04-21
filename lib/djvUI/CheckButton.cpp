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

#include <djvUI/CheckButton.h>

#include <djvAV/Render2D.h>

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
                const float buttonWidth  = 1.f;
                const float buttonHeight = 1.f;

            } // namespace

            struct Check::Private
            {
                float lineHeight = 0.f;
            };

            void Check::_init(Context * context)
            {
                IButton::_init(context);

                setClassName("djv::UI::Button::Check");
                setButtonType(ButtonType::Toggle);
                setHAlign(HAlign::Left);
            }

            Check::Check() :
                _p(new Private)
            {}

            Check::~Check()
            {}

            std::shared_ptr<Check> Check::create(Context * context)
            {
                auto out = std::shared_ptr<Check>(new Check);
                out->_init(context);
                return out;
            }

            void Check::_preLayoutEvent(Event::PreLayout & event)
            {
                DJV_PRIVATE_PTR();
                auto style = _getStyle();
                const float m = style->getMetric(Style::MetricsRole::MarginSmall);
                auto fontSystem = _getFontSystem();
                const auto fontMetrics = fontSystem->getMetrics(style->getFontInfo(AV::Font::faceDefault, Style::MetricsRole::FontMedium)).get();
                p.lineHeight = static_cast<float>(fontMetrics.lineHeight);
                glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                minimumSize.x = p.lineHeight * buttonWidth + m * 2.f;
                minimumSize.y = p.lineHeight + m * 2.f;
                _setMinimumSize(minimumSize);
            }

            void Check::_paintEvent(Event::Paint & event)
            {
                Widget::_paintEvent(event);

                DJV_PRIVATE_PTR();
                auto style = _getStyle();
                const float b = style->getMetric(Style::MetricsRole::Border);
                const float m = style->getMetric(Style::MetricsRole::MarginSmall);
                const BBox2f & g = getGeometry();
                const glm::vec2 c = g.getCenter();

                BBox2f g1;
                g1.min.x = ceilf(c.x - (p.lineHeight * buttonWidth) / 2.f);
                g1.min.y = ceilf(c.y - (p.lineHeight * buttonHeight) / 2.f);
                g1.max.x = g1.min.x + p.lineHeight * buttonWidth;
                g1.max.y = g1.min.y + p.lineHeight * buttonHeight;
                auto render = _getRender();
                render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Border)));
                render->drawRect(g1);

                g1 = g1.margin(-b);
                render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Trough)));
                render->drawRect(g1);

                if (_isToggled())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(getCheckedColorRole())));
                    render->drawRect(g1);
                }
                if (_isPressed())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(getPressedColorRole())));
                    render->drawRect(g1);
                }
                else if (_isHovered())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(getHoveredColorRole())));
                    render->drawRect(g1);
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
