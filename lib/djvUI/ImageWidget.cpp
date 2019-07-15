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

#include <djvUI/Style.h>

#include <djvAV/Image.h>
#include <djvAV/Render2D.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ImageWidget::Private
        {
            std::shared_ptr<AV::Image::Image> image;
            MetricsRole sizeRole = MetricsRole::None;
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

        const std::shared_ptr<AV::Image::Image> & ImageWidget::getImage() const
        {
            return _p->image;
        }

        void ImageWidget::setImage(const std::shared_ptr<AV::Image::Image> & value)
        {
            _p->image = value;
            _resize();
        }

        MetricsRole ImageWidget::getSizeRole() const
        {
            return _p->sizeRole;
        }

        void ImageWidget::setSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.sizeRole)
                return;
            p.sizeRole = value;
            _resize();
        }

        void ImageWidget::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            glm::vec2 size(0.f, 0.f);
            const auto& style = _getStyle();
            if (p.sizeRole != MetricsRole::None)
            {
                size.x = style->getMetric(p.sizeRole);
                size.y = ceilf(size.x / 2.f);
                if (p.image)
                {
                    const float aspectRatio = p.image->getAspectRatio();
                    if (aspectRatio > 0.f)
                    {
                        size.y = ceilf(size.x / aspectRatio);
                    }
                }
            }
            else if (p.image)
            {
                size.x = p.image->getWidth();
                size.y = p.image->getHeight();
            }
            _setMinimumSize(size + getMargin().getSize(style));
        }

        void ImageWidget::_paintEvent(Event::Paint & event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            if (p.image)
            {
                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                const glm::vec2 c = g.getCenter();
                const auto& info = p.image->getInfo();
                glm::vec2 size = glm::vec2(info.size.w, info.size.h);
                glm::vec2 pos = glm::vec2(0.f, 0.f);
                switch (getHAlign())
                {
                case HAlign::Center:
                    pos.x = ceilf(c.x - size.x / 2.f);
                    break;
                case HAlign::Fill:
                    pos.x = g.min.x;
                    size.x = g.w();
                    break;
                case HAlign::Left:   pos.x = g.min.x; break;
                case HAlign::Right:  pos.x = g.max.x - size.x; break;
                default: break;
                }
                switch (getVAlign())
                {
                case VAlign::Center:
                    pos.y = ceilf(c.y - size.y / 2.f);
                    break;
                case VAlign::Fill:
                    pos.y = g.min.y;
                    size.y = g.h();
                    break;
                case VAlign::Top:    pos.y = g.min.y; break;
                case VAlign::Bottom: pos.y = g.max.y - size.y; break;
                default: break;
                }
                auto render = _getRender();
                render->setFillColor(AV::Image::Color(1.f, 1.f, 1.f, getOpacity(true)));
                AV::Render::ImageOptions options;
                options.cache = AV::Render::ImageCache::Dynamic;
                glm::mat3x3 m(1.f);
                m = glm::translate(m, pos);
                m = glm::scale(m, glm::vec2(size.x / static_cast<float>(info.size.w), size.y / static_cast<float>(info.size.h)));
                render->pushTransform(m);
                render->drawImage(p.image, glm::vec2(0.f, 0.f), options);
                render->popTransform();
            }
        }

    } // namespace UI
} // namespace djv
