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

#include <djvUI/ImageWidget.h>

#include <djvUI/System.h>

#include <djvAV/Image.h>
#include <djvAV/Render2DSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ImageWidget::Private
        {
            std::shared_ptr<AV::Image> image;
            size_t hash = 0;
        };

        void ImageWidget::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::UI::ImageWidget");
        }

        ImageWidget::ImageWidget() :
            _p(new Private)
        {}

        ImageWidget::~ImageWidget()
        {}

        std::shared_ptr<ImageWidget> ImageWidget::create(Context * context)
        {
            auto out = std::shared_ptr<ImageWidget>(new ImageWidget);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<AV::Image>& ImageWidget::getImage() const
        {
            return _p->image;
        }

        void ImageWidget::setImage(const std::shared_ptr<AV::Image>& value, size_t hash)
        {
            _p->image = value;
            _p->hash = hash;
        }

        void ImageWidget::preLayoutEvent(PreLayoutEvent& event)
        {
            if (auto style = _getStyle().lock())
            {
                const float t = style->getMetric(UI::MetricsRole::Thumbnail);
                _setMinimumSize(glm::vec2(t, t) + getMargin().getSize(style));
            }
        }

        void ImageWidget::paintEvent(PaintEvent& event)
        {
            Widget::paintEvent(event);
            if (auto render = _getRenderSystem().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);
                    const glm::vec2 c = g.getCenter();

                    // Draw the image.
                    if (_p->image)
                    {
                        const glm::vec2& size = _p->image->getSize();
                        glm::vec2 pos = glm::vec2(0.f, 0.f);
                        switch (getHAlign())
                        {
                        case HAlign::Center:
                        case HAlign::Fill:   pos.x = c.x - size.x / 2.f; break;
                        case HAlign::Left:   pos.x = g.min.x; break;
                        case HAlign::Right:  pos.x = g.max.x - size.x; break;
                        default: break;
                        }
                        switch (getVAlign())
                        {
                        case VAlign::Center:
                        case VAlign::Fill:   pos.y = c.y - size.y / 2.f; break;
                        case VAlign::Top:    pos.y = g.min.y; break;
                        case VAlign::Bottom: pos.y = g.max.y - size.y; break;
                        default: break;
                        }
                        render->setFillColor(AV::Color(1.f, 1.f, 1.f, getOpacity(true)));
                        render->drawImage(_p->image, pos, true, _p->hash);
                    }
                }
            }
        }

    } // namespace UI
} // namespace djv
