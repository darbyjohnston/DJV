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

#include <djvUI/ColorSwatch.h>

#include <djvAV/Render2DSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ColorSwatch::Private
        {
            AV::Image::Color color;
            MetricsRole swatchSize = MetricsRole::Swatch;
        };

        void ColorSwatch::_init(Context * context)
        {
            Widget::_init(context);
            
            setClassName("djv::UI::ColorSwatch");
        }
        
        ColorSwatch::ColorSwatch() :
            _p(new Private)
        {}

        ColorSwatch::~ColorSwatch()
        {}

        std::shared_ptr<ColorSwatch> ColorSwatch::create(Context * context)
        {
            auto out = std::shared_ptr<ColorSwatch>(new ColorSwatch);
            out->_init(context);
            return out;
        }

        std::shared_ptr<ColorSwatch> ColorSwatch::create(const AV::Image::Color& color, Context * context)
        {
            auto out = ColorSwatch::create(context);
            out->setColor(color);
            return out;
        }

        const AV::Image::Color& ColorSwatch::getColor() const
        {
            return _p->color;
        }

        void ColorSwatch::setColor(const AV::Image::Color& value)
        {
            _p->color = value;
        }

        MetricsRole ColorSwatch::getSwatchSize() const
        {
            return _p->swatchSize;
        }

        void ColorSwatch::setSwatchSize(MetricsRole value)
        {
            _p->swatchSize = value;
        }

        void ColorSwatch::preLayoutEvent(Event::PreLayout& event)
        {
            if (auto style = _getStyle().lock())
            {
                const float s = style->getMetric(_p->swatchSize);
                _setMinimumSize(glm::vec2(s, s));
            }
        }

        void ColorSwatch::paintEvent(Event::Paint& event)
        {
            Widget::paintEvent(event);
            if (auto render = _getRenderSystem().lock())
            {
                const BBox2f& g = getGeometry();
                render->setFillColor(_getColorWithOpacity(_p->color));
                render->drawRectangle(g);
            }
        }

    } // namespace UI
} // namespace djv
