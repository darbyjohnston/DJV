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

#include <djvUI/ImageWidget.h>

#include <djvAV/Image.h>
#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ImageWidget::Private
        {
            std::shared_ptr<AV::Image::Image> image;
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

        const std::shared_ptr<AV::Image::Image>& ImageWidget::getImage() const
        {
            return _p->image;
        }

        void ImageWidget::setImage(const std::shared_ptr<AV::Image::Image>& value)
        {
            DJV_PRIVATE_PTR();
            p.image = value;
            _resize();
        }

        void ImageWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            if (auto style = _getStyle().lock())
            {
                glm::vec2 size;
                DJV_PRIVATE_PTR();
                if (p.image)
                {
                    size = p.image->getSize();
                }
                _setMinimumSize(size + getMargin().getSize(style));
            }
        }

        void ImageWidget::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            if (auto render = _getRender().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    DJV_PRIVATE_PTR();
                    if (p.image)
                    {
                        const BBox2f& g = getMargin().bbox(getGeometry(), style);
                        const glm::vec2 c = g.getCenter();
                        const glm::vec2& size = p.image->getSize();
                        glm::vec2 pos = glm::vec2(0.f, 0.f);
                        switch (getHAlign())
                        {
                        case HAlign::Center:
                        case HAlign::Fill:   pos.x = ceilf(c.x - size.x / 2.f); break;
                        case HAlign::Left:   pos.x = g.min.x; break;
                        case HAlign::Right:  pos.x = g.max.x - size.x; break;
                        default: break;
                        }
                        switch (getVAlign())
                        {
                        case VAlign::Center:
                        case VAlign::Fill:   pos.y = ceilf(c.y - size.y / 2.f); break;
                        case VAlign::Top:    pos.y = g.min.y; break;
                        case VAlign::Bottom: pos.y = g.max.y - size.y; break;
                        default: break;
                        }
                        render->setFillColor(AV::Image::Color(1.f, 1.f, 1.f, getOpacity(true)));
                        render->drawImage(p.image, BBox2f(pos.x, pos.y, size.x, size.y), AV::Render::ImageCache::Dynamic);
                    }
                }
            }
        }

    } // namespace UI
} // namespace djv
