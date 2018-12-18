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

#include <djvUI/Border.h>

#include <djvUI/StackLayout.h>

#include <djvAV/Render2DSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Border::Private
        {
            MetricsRole borderSize = MetricsRole::Border;
            MetricsRole borderRounded = MetricsRole::None;
            ColorRole borderColor = ColorRole::Border;
            std::shared_ptr<StackLayout> layout;
        };

        void Border::_init(Context * context)
        {
            IContainerWidget::_init(context);
            
            setClassName("djv::UI::Border");

            _p->layout = StackLayout::create(context);
            IContainerWidget::addWidget(_p->layout);
        }

        Border::Border() :
            _p(new Private)
        {}

        Border::~Border()
        {}

        std::shared_ptr<Border> Border::create(Context * context)
        {
            auto out = std::shared_ptr<Border>(new Border);
            out->_init(context);
            return out;
        }

        MetricsRole Border::getBorderSize() const
        {
            return _p->borderSize;
        }
        
        void Border::setBorderSize(MetricsRole value)
        {
            _p->borderSize = value;
        }
        
        MetricsRole Border::getBorderRounded() const
        {
            return _p->borderRounded;
        }
        
        void Border::setBorderRounded(MetricsRole value)
        {
            _p->borderRounded = value;
        }
        
        ColorRole Border::getBorderColorRole() const
        {
            return _p->borderColor;
        }

        void Border::setBorderColorRole(ColorRole value)
        {
            _p->borderColor = value;
        }

        const Margin& Border::getInsideMargin() const
        {
            return _p->layout->getMargin();
        }

        void Border::setInsideMargin(const Margin& value)
        {
            _p->layout->setMargin(value);
        }

        void Border::addWidget(const std::shared_ptr<Widget>& value)
        {
            _p->layout->addWidget(value);
        }

        void Border::removeWidget(const std::shared_ptr<Widget>& value)
        {
            _p->layout->removeWidget(value);
        }

        void Border::clearWidgets()
        {
            _p->layout->clearWidgets();
        }

        float Border::getHeightForWidth(float value) const
        {
            float out = 0.f;
            if (auto style = _getStyle().lock())
            {
                out = _p->layout->getHeightForWidth(value) +
                    style->getMetric(_p->borderSize) * 2.f +
                    getMargin().getHeight(style);
            }
            return out;
        }

        void Border::preLayoutEvent(PreLayoutEvent& event)
        {
            if (auto style = _getStyle().lock())
            {
                _setMinimumSize(_p->layout->getMinimumSize() +
                    style->getMetric(_p->borderSize) * 2.f +
                    getMargin().getSize(style));
            }
        }

        void Border::layoutEvent(LayoutEvent& event)
        {
            if (auto style = _getStyle().lock())
            {
                const BBox2f g = getGeometry().margin(-style->getMetric(_p->borderSize));
                _p->layout->setGeometry(getMargin().bbox(g, style));
            }
        }

        void Border::paintEvent(PaintEvent& event)
        {
            Widget::paintEvent(event);
            if (auto render = _getRenderSystem().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);

                    // Draw the border.
                    const float borderSize = style->getMetric(_p->borderSize);
                    render->setFillColor(_getColorWithOpacity(style->getColor(_p->borderColor)));
                    render->drawRectangle(BBox2f(
                        glm::vec2(g.min.x, g.min.y),
                        glm::vec2(g.max.x, g.min.y + borderSize)));
                    render->drawRectangle(BBox2f(
                        glm::vec2(g.min.x, g.max.y - borderSize),
                        glm::vec2(g.max.x, g.max.y)));
                    render->drawRectangle(BBox2f(
                        glm::vec2(g.min.x, g.min.y + borderSize),
                        glm::vec2(g.min.x + borderSize, g.max.y - borderSize)));
                    render->drawRectangle(BBox2f(
                        glm::vec2(g.max.x - borderSize, g.min.y + borderSize),
                        glm::vec2(g.max.x, g.max.y - borderSize)));
                }
            }
        }

    } // namespace UI
} // namespace djv
