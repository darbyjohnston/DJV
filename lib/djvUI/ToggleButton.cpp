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

#include <djvUI/ToggleButton.h>

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
                const float toggleWidth = 2.2f;
                const float toggleHeight = 1.2f;

            } // namespace

            struct Toggle::Private
            {
                float lineHeight = 0.f;
            };

            void Toggle::_init(Context * context)
            {
                IButton::_init(context);

                setClassName("djv::UI::Button::Toggle");
                setButtonType(ButtonType::Toggle);
                setBackgroundRole(Style::ColorRole::Button);
            }

            Toggle::Toggle() :
                _p(new Private)
            {}

            Toggle::~Toggle()
            {}

            std::shared_ptr<Toggle> Toggle::create(Context * context)
            {
                auto out = std::shared_ptr<Toggle>(new Toggle);
                out->_init(context);
                return out;
            }

            void Toggle::_preLayoutEvent(Event::PreLayout& event)
            {
                if (auto fontSystem = _getFontSystem().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float m = style->getMetric(Style::MetricsRole::Margin);

                        // Use the font size to determine the size of the button.
                        const auto fontMetrics = fontSystem->getMetrics(style->getFontInfo(AV::Font::Info::faceDefault, Style::MetricsRole::FontMedium)).get();
                        _p->lineHeight = static_cast<float>(fontMetrics.lineHeight);

                        // Set the minimum size.
                        glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                        minimumSize.x = _p->lineHeight * toggleWidth + m * 2.f;
                        minimumSize.y = _p->lineHeight + m * 2.f;
                        _setMinimumSize(minimumSize);
                    }
                }
            }

            void Toggle::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                if (auto render = _getRender().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float b = style->getMetric(Style::MetricsRole::Border);
                        const float m = style->getMetric(Style::MetricsRole::Margin);
                        const BBox2f& g = getGeometry();
                        const glm::vec2 c = g.getCenter();

                        BBox2f g1;
                        g1.min.x = ceilf(c.x - (_p->lineHeight * toggleWidth) / 2.f);
                        g1.min.y = ceilf(c.y - (_p->lineHeight * toggleHeight) / 2.f);
                        g1.max.x = g1.min.x + _p->lineHeight * toggleWidth;
                        g1.max.y = g1.min.y + _p->lineHeight * toggleHeight;

                        // Draw the background.
                        BBox2f g2 = g1.margin(-b / 2.f);
                        /*nvg.beginPath();
                        nvg.roundedRect(g2, g2.h() / 2.f);
                        nvg.fillColor(style->getColor(ColorRole::Trough));
                        nvg.fill();
                        nvg.strokeColor(style->getColor(ColorRole::Border));
                        nvg.stroke();*/

                        // Draw the button handle.
                        glm::vec2 pos = glm::vec2(0.f, 0.f);
                        if (isChecked())
                        {
                            pos.x = g1.max.x - g1.h() / 2.f;
                            pos.y = g1.min.y + g1.h() / 2.f;
                        }
                        else
                        {
                            pos.x = g1.min.x + g1.h() / 2.f;
                            pos.y = g1.min.y + g1.h() / 2.f;
                        }
                        /*nvg.beginPath();
                        nvg.circle(pos, g1.h() / 2.f);
                        nvg.fillColor(style->getColor(isChecked() ? getCheckedColorRole() : ColorRole::Button));
                        nvg.fill();*/

                        // Draw the hovered state.
                        /*if (_isHovered())
                        {
                            nvg.beginPath();
                            nvg.circle(pos, g1.h() / 2.f);
                            nvg.fillColor(style->getColor(ColorRole::Hover));
                            nvg.fill();
                        }*/
                    }
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
