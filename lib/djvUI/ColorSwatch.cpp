//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvUI/ColorSwatch.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/Style.h>

#include <djvAV/Render2D.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ColorSwatch::Private
        {
            AV::Image::Color color;
            MetricsRole swatchSizeRole = MetricsRole::Swatch;
            bool border = true;
            Event::PointerID pressedID = Event::invalidID;
            std::function<void(void)> clickedCallback;
        };

        void ColorSwatch::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            setClassName("djv::UI::ColorSwatch");
            setHAlign(HAlign::Left);
            setPointerEnabled(true);
        }
        
        ColorSwatch::ColorSwatch() :
            _p(new Private)
        {}

        ColorSwatch::~ColorSwatch()
        {}

        std::shared_ptr<ColorSwatch> ColorSwatch::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSwatch>(new ColorSwatch);
            out->_init(context);
            return out;
        }

        const AV::Image::Color & ColorSwatch::getColor() const
        {
            return _p->color;
        }

        void ColorSwatch::setColor(const AV::Image::Color & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.color)
                return;
            p.color = value;
            _redraw();
        }

        MetricsRole ColorSwatch::getSwatchSizeRole() const
        {
            return _p->swatchSizeRole;
        }

        void ColorSwatch::setSwatchSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.swatchSizeRole)
                return;
            p.swatchSizeRole = value;
            _resize();
        }

        void ColorSwatch::setBorder(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.border)
                return;
            p.border = value;
            _resize();            
        }

        void ColorSwatch::setClickedCallback(const std::function<void(void)>& value)
        {
            _p->clickedCallback = value;
        }

        bool ColorSwatch::acceptFocus(TextFocusDirection)
        {
            bool out = false;
            if (_p->clickedCallback && isEnabled(true) && isVisible(true) && !isClipped())
            {
                takeTextFocus();
                out = true;
            }
            return out;
        }

        void ColorSwatch::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float b = style->getMetric(MetricsRole::Border);
            const float sw = style->getMetric(p.swatchSizeRole);
            glm::vec2 size = glm::vec2(sw, sw);
            if (p.clickedCallback)
            {
                size += b * 4.F;
            }
            if (p.border)
            {
                size += b * 2.F;
            }
            _setMinimumSize(size);
        }

        void ColorSwatch::_paintEvent(Event::Paint & event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float b = style->getMetric(MetricsRole::Border);
            const BBox2f & g = getGeometry();

            BBox2f g2 = g;
            auto render = _getRender();
            if (p.clickedCallback)
            {
                if (hasTextFocus())
                {
                    render->setFillColor(style->getColor(ColorRole::TextFocus));
                    drawBorder(render, g, b * 2.F);
                }
                g2 = g2.margin(-b * 2.F);
            }

            if (p.border)
            {
                render->setFillColor(style->getColor(ColorRole::Border));
                drawBorder(render, g2, b);
                g2 = g2.margin(-b);
            }

            const float checkerSize = style->getMetric(MetricsRole::Swatch) / 5.F;
            size_t i = 0;
            for (float y = g2.min.y; y < g2.max.y; y += checkerSize, ++i)
            {
                size_t j = i % 2;
                for (float x = g2.min.x; x < g2.max.x; x += checkerSize, ++j)
                {
                    if (0 == (j % 2))
                    {
                        render->setFillColor(AV::Image::Color(.6F, .6F, .6F));
                    }
                    else
                    {
                        render->setFillColor(AV::Image::Color(.4F, .4F, .4F));
                    }
                    render->drawRect(BBox2f(
                        floorf(x),
                        floorf(y),
                        ceilf(checkerSize),
                        ceilf(checkerSize)).intersect(g2));
                }
            }
            
            render->setFillColor(p.color);
            render->drawRect(g2);

            if (isEnabled(true) && p.pressedID != 0)
            {
                render->setFillColor(style->getColor(ColorRole::Pressed));
                render->drawRect(g2);
            }
            else if (isEnabled(true) && _getPointerHover().size() && p.clickedCallback)
            {
                render->setFillColor(style->getColor(ColorRole::Hovered));
                render->drawRect(g2);
            }
        }

        void ColorSwatch::_pointerEnterEvent(Event::PointerEnter& event)
        {
            DJV_PRIVATE_PTR();
            if (p.clickedCallback)
            {
                event.accept();
                if (isEnabled(true))
                {
                    _redraw();
                }
            }
        }

        void ColorSwatch::_pointerLeaveEvent(Event::PointerLeave& event)
        {
            DJV_PRIVATE_PTR();
            if (p.clickedCallback)
            {
                event.accept();
                if (isEnabled(true))
                {
                    _redraw();
                }
            }
        }

        void ColorSwatch::_pointerMoveEvent(Event::PointerMove& event)
        {
            DJV_PRIVATE_PTR();
            if (p.clickedCallback)
            {
                event.accept();
            }
        }

        void ColorSwatch::_buttonPressEvent(Event::ButtonPress& event)
        {
            DJV_PRIVATE_PTR();
            if (p.clickedCallback)
            {
                if (p.pressedID)
                    return;
                event.accept();
                const auto& pointerInfo = event.getPointerInfo();
                p.pressedID = pointerInfo.id;
                takeTextFocus();
                _redraw();
            }
        }

        void ColorSwatch::_buttonReleaseEvent(Event::ButtonRelease& event)
        {
            DJV_PRIVATE_PTR();
            if (p.clickedCallback)
            {
                const auto& pointerInfo = event.getPointerInfo();
                if (pointerInfo.id == p.pressedID)
                {
                    event.accept();
                    p.pressedID = Event::invalidID;
                    const BBox2f& g = getGeometry();
                    const auto& hover = _getPointerHover();
                    const auto i = hover.find(pointerInfo.id);
                    if (i != hover.end() && g.contains(i->second))
                    {
                        p.clickedCallback();
                        _redraw();
                    }
                }
            }
        }

        void ColorSwatch::_keyPressEvent(Event::KeyPress& event)
        {
            Widget::_keyPressEvent(event);
            DJV_PRIVATE_PTR();
            if (p.clickedCallback)
            {
                if (!event.isAccepted() && hasTextFocus())
                {
                    switch (event.getKey())
                    {
                    case GLFW_KEY_ENTER:
                    case GLFW_KEY_SPACE:
                        event.accept();
                        p.clickedCallback();
                        break;
                    case GLFW_KEY_ESCAPE:
                        event.accept();
                        releaseTextFocus();
                        break;
                    default: break;
                    }
                }
            }
        }

        void ColorSwatch::_textFocusEvent(Event::TextFocus&)
        {
            _redraw();
        }

        void ColorSwatch::_textFocusLostEvent(Event::TextFocusLost&)
        {
            _redraw();
        }

    } // namespace UI
} // namespace djv
